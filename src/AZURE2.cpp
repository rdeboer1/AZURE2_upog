/* 
 * AZURE2.cpp
 * Ethan Uberseder, University of Notre Dame, 2011
 * 
 * This file contains the main() function, as well as other C functions used to 
 * initialize the program.  The functions defined here are (mostly) proceedural
 * C, and used to initialize the AZUREMain object from command line input.  All
 * actual calculations are managed by the AZUREMain object, and (almost) all 
 * subsequent routines/classes follow object-oriented C++ programming conventions.
 */


#include "AZUREMain.h"
#include "Config.h"
#include "ECLine.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <readline/readline.h>
#include <readline/history.h>

struct SegPairs {int firstPair; int secondPair;};

/*!
 * This function displays the welcome banner.
 */

void welcomeMessage() {
  std::cout << std::endl
	    << "O--------------------------O----------------------------O" << std::endl
	    << "| #### #### #  # ###  #### | Version 2.0                |" << std::endl
	    << "| #  #    # #  # #  # #    | Object Oriented C++        |" << std::endl
	    << "| ####   #  #  # ###  ##   | June 14, 2011              |" << std::endl
	    << "| #  #  #   #  # # #  #    |                            |" << std::endl
	    << "| #  # ####  ##  #  # #### | University of Notre Dame   |" << std::endl
	    << "O--------------------------O----------------------------O" << std::endl
	    << std::endl;
}

/*!
 * This function prints a message upon successful termination of the program.
 */

void exitMessage() {
  std::cout << std::endl
	    << "Thanks for using AZURE." << std::endl;
}

/*!
 * This function prints the response to the --help command which consists of
 * available runtime options.
 */

void printHelp() {
  std::cout << "Syntax: AZURE2 <options> configfile" << std::endl << std::endl
	    << "Options:" << std::endl
            << std::setw(25) << std::left << "\t--no-readline:" << std::setw(0) << "Do not use readline package." <<  std::endl
            << std::setw(25) << std::left << "\t--no-transform:" << std::setw(0) << "Do not perform initial parameter transformations." << std::endl
            << std::setw(25) << std::left << "\t--use-brune:" << std::setw(0) << "Use the alternative level matrix of C.R. Brune." << std::endl
            << std::setw(25) << std::left << "\t--ignore-externals:" << std::setw(0) << "Ignore external resonant capture amplitude if internal width is zero." << std::endl
            << std::setw(25) << std::left << "\t--use-rmc:" << std::setw(0) << "Use Reich-Moore approximation for capture." << std::endl;
}

/*!
 * This function parses the command line options given, and sets the appropriate variables in the Config
 * structure.  It also reads and parses the configuration file if the appropriate environment
 * variable is set.
 */

bool parseOptions(int argc, char *argv[], Config& configure) {
  bool useReadline=true;
  configure.mask |= TRANSFORM_PARAMETERS;
  configure.mask &= ~USE_BRUNE_FORMALISM;
  configure.mask &= ~IGNORE_ZERO_WIDTHS;
  configure.mask &= ~USE_RMC_FORMALISM;
  configure.configfile="";
  std::vector<std::string> options;
  for(int i=1;i<argc;i++) {
    std::string arg=argv[i];
    if(!arg.empty()&&arg.substr(0,2)=="--") options.push_back(arg);
    else configure.configfile=arg;
  }
  char* optionsFile = getenv("AZURE_OPTIONS_FILE");
  if(optionsFile) {
    std::ifstream in(optionsFile);
    if(in) {
      while(!in.eof()) {
	std::string newOption;
	getline(in,newOption);
	if(!in.eof()) {
	  std::string trimmedOption="";
	  for(std::string::iterator it = newOption.begin();it<newOption.end();it++) 
	    if(*it!=' '&&*it!='\t'&&*it!='\n')
	      trimmedOption+=*it;
	  if(!trimmedOption.empty()&&trimmedOption.substr(0,2)=="--") options.push_back(trimmedOption);
	}
      }
      in.close();
    } else std::cout << "AZURE_OPTIONS_FILE variable set, but file not readable." << std::endl;
  }
  for(std::vector<std::string>::iterator it = options.begin();it<options.end();it++) {
    if(*it=="--help") {
      printHelp();
      std::exit(0);
    } else if(*it=="--no-readline") useReadline=false;
    else if(*it=="--no-transform") configure.mask &= ~TRANSFORM_PARAMETERS;
    else if(*it=="--use-brune") configure.mask |= USE_BRUNE_FORMALISM;
    else if(*it=="--ignore-externals") configure.mask |= IGNORE_ZERO_WIDTHS;
    else if(*it=="--use-rmc") configure.mask |= USE_RMC_FORMALISM;
    else std::cout << "WARNING: Unknown option " << *it << '.' << std::endl;
  }
  return useReadline;
}

/*!
 * This function handles the command shell in AZURE2.  The function will not terminate until the user
 * enters a valid integer option.  Upon successful entry, the integer option is returned.
 */

int commandShell() {
  int command=0;

  std::cout << "Please select from the following options: " << std::endl
	    << "\t1. Data Fit" << std::endl
	    << "\t2. Data Calculate" << std::endl
	    << "\t3. Extrapolate (no data)" << std::endl
	    << "\t4. Perform MINOS Error Analysis" << std::endl
	    << "\t5. Reaction Rate" << std::endl
	    << "\t6. Exit" << std::endl;

  while(command<1||command>6) {
    std::cout << "azure2: ";
    std::string inString;
    getline(std::cin,inString);
    if(inString.empty()) continue;
    std::istringstream in;
    in.str(inString);
    if(!(in>>command)) 
      std::cout << "Please enter an integer." << std::endl;
    else if(command<1||command>6) 
      std::cout << "Invalid option.  Please try again."
		<< std::endl;
  } 
  return command;
}

/*!
 * This function takes the returned option from the commandShell function and
 * sets the appropriate flags in the Config structure.
 */

void processCommand(int command, Config& configure) {
  configure.chiVariance=1.0;
  configure.mask &= ~PERFORM_FIT;
  configure.mask &= ~PERFORM_ERROR_ANALYSIS;
  configure.mask |= CALCULATE_WITH_DATA;
  configure.mask &= ~CALCULATE_REACTION_RATE;

  if(command==1) configure.mask |= PERFORM_FIT;
  else if(command==3) configure.mask &= ~CALCULATE_WITH_DATA;
  else if(command==4) {
    std::cout << std::endl
	      << std::setw(30) << "Allowed Chi-Squared Variance: ";
    std::cin >> configure.chiVariance;
    configure.mask |= PERFORM_FIT;
    configure.mask |= PERFORM_ERROR_ANALYSIS;
  } else if(command==5) {
    configure.mask &= ~CALCULATE_WITH_DATA;
    configure.mask |= CALCULATE_REACTION_RATE;
  } else if(command==6) {
    exitMessage();
    std::exit(0);
  }
}

/*!
 * This function prompts for a parameter file and sets the corresponding configure
 * flags and variables based on the user response.
 */

void getParameterFile(bool useReadline, Config& configure) {
  configure.mask &= ~USE_PREVIOUS_PARAMETERS;
  bool validInfile=false;
  std::cout << std::endl;
  if(!useReadline) std::cout << "External Parameter File (leave blank for new file): ";
  while(!validInfile) {
    std::string inFile;
    if(!useReadline) getline(std::cin,inFile);
    else {
      char *line = readline("External Parameter File (leave blank for new file): ");
      inFile=line;
      size_t endpos = inFile.find_last_not_of(" \t");
      if( std::string::npos != endpos ) inFile = inFile.substr( 0, endpos+1 );
      if(line && *line) add_history(line);
      free(line);
    }
    if(!inFile.empty()) {
      std::ifstream in;
      in.open(inFile.c_str());
      if(in) {
	validInfile=true;
	configure.mask |= USE_PREVIOUS_PARAMETERS;
	configure.paramfile=inFile;
	in.close();
      }
      in.clear();
    } else validInfile=true;
    if(!validInfile) {
      std::cout << "Cannot Read From " << inFile << ". Please reenter file." << std::endl;
      if(!useReadline) std::cout << "External Parameter File (leave blank for new file): ";
    }
  }
}

/*!
 * This function reads the segment file, and stores the active entrance and exit pair keys
 * for cross reference with the External capture file.  Only if an active external capture segment
 * is required is the user prompted for an external integrals file.
 */

void readSegmentFile(const Config& configure,std::vector<SegPairs>& segPairs) {
  std::ifstream in;
  std::string startTag,stopTag;
  if(configure.mask & CALCULATE_WITH_DATA) {
    startTag="<segmentsData>";
    stopTag="</segmentsData>";
  } else {
    startTag="<segmentsTest>";
    stopTag="</segmentsTest>";    
  }
  in.open(configure.configfile.c_str());
  if(in) {
    std::string line="";
    while(line!=startTag&&!in.eof()) getline(in,line);
    if(line==startTag) {
      line="";
      int isActive,firstPair,secondPair;
      while(line!=stopTag&&!in.eof()) {
	getline(in,line);
	bool empty=true;
	for(unsigned int i=0;i<line.size();++i) 
	  if(line[i]!=' '&&line[i]!='\t') {
	    empty=false;
	    break;
	  }
	if(empty==true) continue;
	if(line!=stopTag&&!in.eof()) {
	  std::istringstream stm;
	  stm.str(line);
	  stm >> isActive >> firstPair >> secondPair;
	  if(!(stm.rdstate() & (std::stringstream::failbit | std::stringstream::badbit))&&isActive==1) {
	    SegPairs tempSet={firstPair,secondPair};
	    segPairs.push_back(tempSet);
	  }
	}
      }
      if(line!=stopTag) {
	std::cout << "Problem reading segments. Check configuration file." << std::endl;
	std::exit(1);
      }
    } else {
      std::cout << "Problem reading segments. Check configuration file." << std::endl;
      std::exit(1);
    }
    in.close();
  } else {
    std::cout << "Cannot read segments. Check configuration file." << std::endl;
    std::exit(1);
  }
  in.clear();
}

/*!
 * If reaction rate is desired, the user may specify a file containing temperatures for the calculation.
 * This function prompts for that file name, checks for access, and stores path.
 */

void getTemperatureFile(bool useReadline, std::string& temperatureFile) {
  bool validInfile=false;
  if(!useReadline) std::cout << std::setw(38) << "Temperature File Name: ";
  while(!validInfile) {
    std::string inFile;
    if(!useReadline) getline(std::cin,inFile);
    else {
      char *line = readline("               Temperature File Name: ");
      inFile=line;
      size_t endpos = inFile.find_last_not_of(" \t");
      if( std::string::npos != endpos ) inFile = inFile.substr( 0, endpos+1 );
      if(line && *line) add_history(line);
      free(line);
    }
    if(!inFile.empty()) {
      std::ifstream in;
      in.open(inFile.c_str());
      if(in) {
	validInfile=true;
	temperatureFile=inFile;
	in.close();
      }
      in.clear();
    }
    if(!validInfile) {
      std::cout << "Cannot Read From " << inFile << ". Please reenter file." << std::endl;
      if(!useReadline) std::cout << "               Temperature File Name: ";
    }
  }
}

/*!
 * This function  prompts the user for the required parameters if reaction rate is 
 * to be calculated.
 */

void getRateParams(RateParams& rateParams, std::vector<SegPairs>& segPairs,bool useReadline) {
  rateParams.entrancePair=0;
  rateParams.exitPair=0;
  rateParams.minTemp=0.;
  rateParams.maxTemp=0.;
  rateParams.tempStep=0.;
  while(rateParams.entrancePair==rateParams.exitPair){
    std::cout << std::endl;
    std::cout << std::setw(38) << "Reaction Rate Entrance Pair: ";
    std::string inString;
    getline(std::cin,inString);
    std::istringstream stm;
    stm.str(inString);
    stm >> rateParams.entrancePair;
    stm.clear();inString="";
    std::cout << std::setw(38) << "Reaction Rate Exit Pair: ";
    getline(std::cin,inString);
    stm.str(inString);
    stm >> rateParams.exitPair;
    if(rateParams.entrancePair==rateParams.exitPair) 
      std::cout << "Cannot calculate rate for elastic scattering." << std::endl;
  }
  SegPairs tempSet={rateParams.entrancePair,rateParams.exitPair};
  segPairs.push_back(tempSet);
  std::cout << std::setw(38) << "Use temperatures from file (yes/no): ";
  bool goodAnswer = false;
  std::string fileAnswer="";
  while(!goodAnswer) {   
    getline(std::cin,fileAnswer);
    std::string trimmedAnswer;
    for(int i =0;i<fileAnswer.length();i++)
      if(fileAnswer[i]!=' '&&fileAnswer[i]!='\t'&&fileAnswer[i]!='\n')
	trimmedAnswer+=fileAnswer[i];
    if(trimmedAnswer!="yes"&&trimmedAnswer!="no") 
     std::cout << "Please type 'yes' or 'no'." <<std::endl;
    else {
      goodAnswer = true;
      fileAnswer=trimmedAnswer;
    }
  }
  rateParams.useFile = (fileAnswer=="yes") ? (true) : (false); 
  if(rateParams.useFile) getTemperatureFile(useReadline,rateParams.temperatureFile);
  else {
    std::cout << std::setw(38) << "Reaction Rate Min Temp [GK]: ";
    std::cin >> rateParams.minTemp;
    std::cout << std::setw(38) << "Reaction Rate Max Temp [GK]: ";
    std::cin >> rateParams.maxTemp;
    std::cout << std::setw(38) << "Reaction Rate Temp Step [GK]: ";
    std::cin >> rateParams.tempStep;
  }
}

/*!
 * This function checks the external capture file against a vector of segment key 
 * pairs. Only if the calculation includes external capture segments is the user
 * prompted for an integrals file.  The appropriate configure flag is set here.
 */

void checkExternalCapture(Config& configure, const std::vector<SegPairs>& segPairs) {
  configure.mask &= ~USE_EXTERNAL_CAPTURE;
  std::ifstream in;
  in.open(configure.configfile.c_str());
  if(in) {
    std::string line="";
    while(line!="<externalCapture>"&&!in.eof()) getline(in,line);
    if(line=="<externalCapture>") {
      line="";
      while(line!="</externalCapture>"&&!in.eof()&&(configure.mask ^ USE_EXTERNAL_CAPTURE)) {
	getline(in,line);
	bool empty=true;
	for(unsigned int i=0;i<line.size();++i) 
	  if(line[i]!=' '&&line[i]!='\t') {
	    empty=false;
	    break;
	  }
	if(empty==true) continue;
	if(line!="</externalCapture>"&&!in.eof()) {
	  std::istringstream stm;
	  stm.str(line);
	  ECLine tempECLine(stm);
	  if(!(stm.rdstate() & (std::stringstream::failbit | std::stringstream::badbit))) {
	    if(tempECLine.isActive()!=0) {
	      for(int i=0;i<segPairs.size();i++) {
		if(tempECLine.exitKey()==segPairs[i].secondPair) {
		  configure.mask |= USE_EXTERNAL_CAPTURE;
		  break;
		}
	      }
	    }
	  } else {
	    std::cout << "Problem reading external capture. Check configuration file." << std::endl;
	    std::exit(1);
	  }
	}
      }
    } else {
      std::cout << "Problem reading external capture. Check configuration file." << std::endl;
      std::exit(1);
    }
    in.close();
  } else {
    std::cout << "Cannot read external capture. Check configuration file." << std::endl;
    std::exit(1);
  }
  in.clear();
  if((configure.mask & USE_EXTERNAL_CAPTURE)&&(configure.mask & USE_RMC_FORMALISM)) {
    std:: cout << "WARNING: External capture is not compatible with Reich-Moore.  Ignoring external capture." 
	       << std::endl;
    configure.mask &= ~USE_EXTERNAL_CAPTURE;
  }
}

/*!
 * This function promps the user for an external capture integrals file, checks for it's validity, 
 * and stores the path. 
 */

void getExternalCaptureFile(bool useReadline, Config& configure) {
  configure.mask &= ~USE_PREVIOUS_INTEGRALS;
  if((configure.mask & USE_EXTERNAL_CAPTURE)&&(configure.mask ^ CALCULATE_REACTION_RATE)) {
    std::cout << std::endl;
    if(!useReadline) std::cout << "External Capture Amplitude File (leave blank for new file): ";
    bool validInfile=false;
    while(!validInfile) {
      std::string inFile;
      if(!useReadline) getline(std::cin,inFile);
      else {
	char *line = readline("External Capture Amplitude File (leave blank for new file): ");
	inFile=line;
	size_t endpos = inFile.find_last_not_of(" \t");	
	if( std::string::npos != endpos ) inFile = inFile.substr( 0, endpos+1 );
	if(line && *line) add_history(line);
	free(line);
      }
      if(!inFile.empty()) {
	std::ifstream in;
	in.open(inFile.c_str());
	if(in) {
	  validInfile=true;
	  configure.mask |= USE_PREVIOUS_INTEGRALS;
	  configure.integralsfile=inFile;
	  in.close();
	}
	in.clear();
      } else validInfile=true;
      if(!validInfile) {
	std::cout << "Cannot Read From " << inFile << ". Please reenter file." << std::endl;
	if(!useReadline) std::cout << "External Capture Amplitude File (leave blank for new file): ";
      }
    }
  } 
}

/*!
 * This function prints a brief start message describing the type of calculation that will be performed.
 */

void startMessage(const Config& configure) {
  if(configure.mask & PERFORM_ERROR_ANALYSIS) std::cout << std::endl
				       << "Calling AZURE in error analysis mode..." << std::endl;
  else if(configure.mask & CALCULATE_REACTION_RATE) std::cout << std::endl
					<< "Calling AZURE in reaction rate mode..." << std::endl;  
  else if(configure.mask ^ CALCULATE_WITH_DATA) std::cout << std::endl
					 << "Calling AZURE in extrapolate mode..." << std::endl; 
  else if(configure.mask & PERFORM_FIT) std::cout << std::endl
					  << "Calling AZURE in fit mode..." << std::endl;
  else  std::cout << std::endl
		  << "Calling AZURE in calculate mode..." << std::endl;
}

/*!
 * This is the main function. All the above initialization functions are called from here.
 * The AZUREMain oject is created and called.
 */

int main(int argc,char *argv[]){
  //Create new configuration structure, and parse the command line parameters
  Config configure;
  bool useReadline = parseOptions(argc,argv,configure);

  //Read the parameters from the runtime configuration file
  if(configure.configfile.empty()) {
    std::cout << "A valid configuration file must be specified." << std::endl
	      << "\tSyntax: AZURE2 <options> configfile" << std::endl;
    return -1;
  }
  if(ReadConfigFile(configure)==-1) {
    std::cout << "Could not open " << configure.configfile << ".  Check that file exists." 
	      << std::endl;
    return -1;
  }
#ifndef NO_STAT
  else if(CheckForInputFiles(configure) == -1) return -1;
#endif
  if((configure.mask & USE_RMC_FORMALISM) && (configure.mask & USE_BRUNE_FORMALISM)) {
    std::cout << "WARNING: --use-brune is incompatible with --use-rmc. Ignoring --use-brune." << std::endl;
    configure.mask &= ~USE_BRUNE_FORMALISM;
  }
  if((configure.mask & USE_BRUNE_FORMALISM)||(configure.mask & IGNORE_ZERO_WIDTHS)) configure.mask |= USE_AMATRIX;

  //Print welcome message
  welcomeMessage();

  //Read and process command, setting appropriate configuration flags
  processCommand(commandShell(),configure);
  
  //Open history file for readline
  if(useReadline) read_history("./.azure_history");
  
  //Read the external parameter file to be used, if any
  getParameterFile(useReadline,configure);
  
  //Parse the segment files for entrance,exit pairs
  std::vector<SegPairs> segPairs;
  if(configure.mask ^ CALCULATE_REACTION_RATE) readSegmentFile(configure,segPairs);
  else getRateParams(configure.rateParams,segPairs,useReadline);

  //Check if the entrance,exit pairs are in the external capture file
  // If so, external capture will be needed
  checkExternalCapture(configure,segPairs);
  
  //Read the external capture file name to be used, if any
  getExternalCaptureFile(useReadline,configure);
   
  //Create instance of main AZURE function, print start message,
  // and execute
  AZUREMain azureMain(configure);
  startMessage(configure);
  azureMain();
  
  //Print exit message
  exitMessage();

  //Write readline history file
  if(useReadline) write_history("./.azure_history");
  
  return 0;
}
