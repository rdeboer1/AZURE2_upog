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

void welcomeMessage() {
  std::cout << std::endl
	    << "O--------------------------O----------------------------O" << std::endl
	    << "| #### #### #  # ###  #### | Version 2.0                |" << std::endl
	    << "| #  #    # #  # #  # #    | Object Oriented C++        |" << std::endl
	    << "| ####   #  #  # ###  ##   | October 12, 2010           |" << std::endl
	    << "| #  #  #   #  # # #  #    |                            |" << std::endl
	    << "| #  # ####  ##  #  # #### | University of Notre Dame   |" << std::endl
	    << "O--------------------------O----------------------------O" << std::endl
	    << std::endl;
}

void exitMessage() {
  std::cout << std::endl
	    << "Thanks for using AZURE." << std::endl;
}

void printHelp() {
  std::cout << "Syntax: AZURE2 options configfile" << std::endl << std::endl
	    << "Options:" << std::endl
            << std::setw(25) << std::left << "\t--no-readline:" << std::setw(0) << "Do not use readline package." <<  std::endl
            << std::setw(25) << std::left << "\t--no-transform:" << std::setw(0) << "Do not perform initial parameter transformations." << std::endl
            << std::setw(25) << std::left << "\t--use-brune:" << std::setw(0) << "Use the alternative level matrix of C.R. Brune." << std::endl
            << std::setw(25) << std::left << "\t--ignore-externals:" << std::setw(0) << "Ignore external resonant capture amplitude if internal width is zero." << std::endl
            << std::setw(25) << std::left << "\t--use-rmc:" << std::setw(0) << "Use Reich-Moore approximation for capture." << std::endl;
}

bool parseOptions(int argc, char *argv[], Config& configure) {
  bool useReadline=true;
  configure.transformParams=true;
  configure.isBrune=false;
  configure.ignoreExternals=false;
  configure.useRMC=false;
  if(argc>2) {
    for(int i=1; i<argc-1; i++) {
      std::string arg=argv[i];
      if(arg=="--help") {
	printHelp();
	std::exit(0);
      } else if(arg=="--no-readline") useReadline=false;
      else if(arg=="--no-transform") configure.transformParams=false;
      else if(arg=="--use-brune") configure.isBrune=true;
      else if(arg=="--ignore-externals") configure.ignoreExternals=true;
      else if(arg=="--use-rmc") configure.useRMC=true;
      else std::cout << "WARNING: Unknown option " << arg << '.' << std::endl;
    }
    configure.configfile=argv[argc-1];
  } else {
    if(std::string(argv[1])=="--help") {
	printHelp();
	std::exit(0);
    } else configure.configfile=argv[1];
  }
  return useReadline;
}

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

void processCommand(int command, Config& configure) {
  configure.chiVariance=1.0;
  configure.performFit=false;
  configure.performError=false;
  configure.withData=true;
  configure.calcRate=false;

  if(command==1) configure.performFit=true;
  else if(command==3) configure.withData=false;
  else if(command==4) {
    std::cout << std::endl
	      << std::setw(30) << "Allowed Chi-Squared Variance: ";
    std::cin >> configure.chiVariance;
    configure.performFit=true;
    configure.performError=true;
  } else if(command==5) {
    configure.withData=false;
    configure.calcRate=true;
  } else if(command==6) {
    exitMessage();
    std::exit(0);
  }
}

void getParameterFile(bool useReadline, Config& configure) {
  configure.oldParameters=false;
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
	configure.oldParameters=true;
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

void readSegmentFile(const Config& configure,std::vector<SegPairs>& segPairs) {
  std::ifstream in;
  std::string startTag,stopTag;
  if(configure.withData) {
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

void getRateParams(RateParams& rateParams, std::vector<SegPairs>& segPairs) {
  rateParams.entrancePair=0;
  rateParams.exitPair=0;
  rateParams.minTemp=0.;
  rateParams.maxTemp=0.;
  rateParams.tempStep=0.;
  while(rateParams.entrancePair==rateParams.exitPair){
    std::cout << std::endl;
    std::cout << std::setw(30) << "Reaction Rate Entrance Pair: ";
    std::cin >> rateParams.entrancePair;
    std::cout << std::setw(30) << "Reaction Rate Exit Pair: ";
    std::cin >> rateParams.exitPair;
    if(rateParams.entrancePair==rateParams.exitPair) 
      std::cout << "Cannot calculate rate for elastic scattering." << std::endl;
  }
  SegPairs tempSet={rateParams.entrancePair,rateParams.exitPair};
  segPairs.push_back(tempSet);
  std::cout << std::setw(30) << "Reaction Rate Min Temp [GK]: ";
  std::cin >> rateParams.minTemp;
  std::cout << std::setw(30) << "Reaction Rate Max Temp [GK]: ";
  std::cin >> rateParams.maxTemp;
  std::cout << std::setw(30) << "Reaction Rate Temp Step [GK]: ";
  std::cin >> rateParams.tempStep;
}

void checkExternalCapture(Config& configure, const std::vector<SegPairs>& segPairs) {
  configure.isEC=false;
  std::ifstream in;
  in.open(configure.configfile.c_str());
  if(in) {
    std::string line="";
    while(line!="<externalCapture>"&&!in.eof()) getline(in,line);
    if(line=="<externalCapture>") {
      line="";
      while(line!="</externalCapture>"&&!in.eof()&&!configure.isEC) {
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
	    if(tempECLine.isEC()!=0) {
	      for(int i=0;i<segPairs.size();i++) {
		if(tempECLine.exitKey()==segPairs[i].secondPair) {
		  configure.isEC=true;
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
  if(configure.isEC&&configure.useRMC) {
    std:: cout << "WARNING: External capture is not compatible with Reich-Moore.  Ignoring external capture." 
	       << std::endl;
    configure.isEC=false;
  }
}

void getExternalCaptureFile(bool useReadline, Config& configure) {
  configure.oldECFile=false;
  if(configure.isEC&&!configure.calcRate) {
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
	  configure.oldECFile=true;
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

void startMessage(const Config& configure) {
  if(configure.performError) std::cout << std::endl
				       << "Calling AZURE in error analysis mode..." << std::endl;
  else if(configure.calcRate) std::cout << std::endl
					<< "Calling AZURE in reaction rate mode..." << std::endl;  
  else if(!configure.withData) std::cout << std::endl
					 << "Calling AZURE in extrapolate mode..." << std::endl; 
  else if(configure.performFit) std::cout << std::endl
					  << "Calling AZURE in fit mode..." << std::endl;
  else  std::cout << std::endl
		  << "Calling AZURE in calculate mode..." << std::endl;
}

/*!
 * \mainpage AZURE R-Matrix Package
 * AZURE is created to be general A-/R- Matrix program for the analysis of
 * data relevant to nuclear astrophysics.  This release of AZURE has been 
 * completely rewritten in object-oriented C++.  The documentation contained 
 * within these pages should serve as an introduction to the object structure of
 * AZURE Version 2.0 (AZURE2).
 * Most mathematical functions in AZURE2 utilize the GNU Scientific Libraries (GSL),
 * while the minimization routines are handled by Minuit2 (C++ version of Minuit). AZURE2
 * uses OpenMP to parallel many calculations and fitting, therefore the performance boost 
 * of AZURE2 will be significant when used on multicore multiprocessor machines.
 */

int main(int argc,char *argv[]){

  if(argc<2) {
    std::cout << "Too few arguments.  Configuration file must be specified." << std::endl
	      << "\tSyntax: AZURE2 configfile" << std::endl;
    return -1;
  } 

  //Create new configuration structure, and parse the command line parameters
  Config configure;
  bool useReadline = parseOptions(argc,argv,configure);

  //Read the parameters from the runtime configuration file
  if(ReadConfigFile(configure)==-1) {
    std::cout << "Could not open " << configure.configfile << ".  Check that file exists." 
	      << std::endl;
    return -1;
  }
#ifndef NO_STAT
  else if(CheckForInputFiles(configure) == -1) return -1;
#endif
  if(configure.useRMC && configure.isBrune) {
    std::cout << "WARNING: --use-brune is incompatible with --use-rmc. Ignoring --use-brune." << std::endl;
    configure.isBrune=false;
  }
  if(configure.isBrune||configure.ignoreExternals) configure.isAMatrix=true;

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
  if(!configure.calcRate) readSegmentFile(configure,segPairs);
  else getRateParams(configure.rateParams,segPairs);

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
