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
 * \mainpage AZURE R-Matrix Package
 * AZURE is created to be general A-/R- Matrix program for the analysis of
 * data relevant to nuclear astrophysics.  This release of AZURE has been 
 * completely rewritten in object-oriented C++.  The documentation contained 
 * within these pages should serve as an introduction to the object structure of
 * AZURE Version 2.0.
 * Most mathematical functions in AZURE utilized the GNU Scientific Libraries (GSL),
 * while the minimization routines are handled by Minuit2 (C++ version of Minuit). Minuit2
 * uses OpenMP to parallel the fitting process, therefore the performance boost of AZURE
 * will be significant when used on multicore multiprocessor machines.
 */

int main(int argc,char *argv[]){

  bool useReadline=true;
  Config configure;

  if(argc<2) {
    std::cout << "Too few arguments.  Configuration file must be specified." << std::endl
	      << "\tSyntax: azure2 configfile" << std::endl;
    return -1;
  } else if(argc>2) {
    std::string arg=argv[1];
    if(arg=="--no-readline") useReadline=false;
    else std::cout << "Unknown option " << arg << '.' << std::endl;
    configure.configfile=argv[argc-1];
  } else configure.configfile=argv[1];

  if(useReadline) read_history("./.azure_history");
  std::cout << std::endl
	    << "O--------------------------O----------------------------O" << std::endl
	    << "| #### #### #  # ###  #### | Version 2.0                |" << std::endl
	    << "| #  #    # #  # #  # #    | Object Oriented C++        |" << std::endl
	    << "| ####   #  #  # ###  ##   |                            |" << std::endl
	    << "| #  #  #   #  # # #  #    | Ethan Uberseder            |" << std::endl
	    << "| #  # ####  ##  #  # #### | University of Notre Dame   |" << std::endl
	    << "O--------------------------O----------------------------O" << std::endl
	    << std::endl;
  
  if(ReadConfigFile(configure)==-1) {
    std::cout << "Could not open " << configure.configfile << ".  Check that file exists." 
	      << std::endl;
    return -1;
  }
#ifndef NO_STAT
  else if(CheckForInputFiles(configure) == -1) return -1;
#endif

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
    while(!(std::cin >> command)) {
      std::cin.clear();
      std::cin.ignore(1000,'\n');
      std::cout << "Please enter an integer." << std::endl;
      std::cout << "azure2: ";
    }
    if(command<1||command>6) {
      std::cout << "Invalid option.  Please try again."
		<< std::endl;
    }
  }
  
  if(command!=6) {
    int rateEntrancePair=0;
    int rateExitPair=0;
    double rateMinTemp=0.0;
    double rateMaxTemp=0.0;
    double rateTempStep=0.0;
    double chiVariance=1.0;
    bool performFit=false;
    bool performError=false;
    bool oldParameters=false;
    bool validInfile=false;
    bool withData=true;
    bool calcRate=false;
    std::string inFile;
    std::ifstream in;
    std::cout << std::endl;
    if(!useReadline) std::cout << "External Parameter File (leave blank for new file): ";
    std::cin.ignore(1000,'\n');
    while(!validInfile) {
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
	in.open(inFile.c_str());
	if(in) {
	  validInfile=true;
	  oldParameters=true;
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
    bool isEC=false;
    std::vector<SegPairs> segPairs;
    if(command<=4) {
      if(command<=2||command==4) in.open(configure.segfile.c_str());
      else if(command==3) in.open(configure.extrapfile.c_str());
      if(in) {
	int isActive,firstPair,secondPair;
	std::string dummy;
	getline(in,dummy);
	while(!in.eof()) {
	  in >> isActive >> firstPair >> secondPair;getline(in,dummy);
	  if(!in.eof()&&isActive==1) {
	    SegPairs tempSet={firstPair,secondPair};
	    segPairs.push_back(tempSet);
	  }
	}
	in.close();
      } else {
	std::cout << "Cannot read from segment file. Check configuration file." << std::endl;
	return -1;
      }
      in.clear();
    } else if(command==5) {
      while(rateEntrancePair==rateExitPair){
	std::cout << std::endl;
	std::cout << std::setw(30) << "Reaction Rate Entrance Pair: ";
	std::cin >> rateEntrancePair;
	std::cout << std::setw(30) << "Reaction Rate Exit Pair: ";
	std::cin >> rateExitPair;
	if(rateEntrancePair==rateExitPair) std::cout << "Cannot calculate rate for elastic scattering." << std::endl;
      }
      SegPairs tempSet={rateEntrancePair,rateExitPair};
      segPairs.push_back(tempSet);
      std::cout << std::setw(30) << "Reaction Rate Min Temp [GK]: ";
      std::cin >> rateMinTemp;
      std::cout << std::setw(30) << "Reaction Rate Max Temp [GK]: ";
      std::cin >> rateMaxTemp;
      std::cout << std::setw(30) << "Reaction Rate Temp Step [GK]: ";
      std::cin >> rateTempStep;
    }
    in.open(configure.ecfile.c_str());
    if(in) {
      std::string dummy;
      getline(in,dummy);
      while(!in.eof()&&!isEC) {
	ECLine tempECLine=ReadECLine(in);
	if(!in.eof()) {
	  if(tempECLine.isdc!=0) {
	    for(int i=0;i<segPairs.size();i++) {
	      if(tempECLine.entrancekey==segPairs[i].firstPair&&
		 tempECLine.exitkey==segPairs[i].secondPair) {
		isEC=true;
		break;
	      }
	    }
	  }
	}
      }
      in.close();
    } else {
      std::cout << "Cannot read from external capture file. Check configuration file." << std::endl;
      return -1;
    }
    in.clear();
    validInfile=false;
    bool oldECFile=false;
    if(isEC&&command!=5) {
      std::cout << std::endl;
      if(!useReadline) std::cout << "External Capture Amplitude File (leave blank for new file): ";
      while(!validInfile) {
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
	    oldECFile=true;
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
    if(command==1) {
      performFit=true;
      std::cout << std::endl
		<< "Calling AZURE in fit mode..." << std::endl;
    } else if(command==2) {
      std::cout << std::endl
		<< "Calling AZURE in calculate mode..." << std::endl;
    } else if(command==3) {
      withData=false;
      std::cout << std::endl
		<< "Calling AZURE in extrapolate mode..." << std::endl;
    } else if(command==4) {
      std::cout << std::endl;
      std::cout << std::setw(30) << "Allowed Chi-Squared Variance: ";
      std::cin >> chiVariance;
      performFit=true;
      performError=true;
      std::cout << std::endl
		<< "Calling AZURE in error analysis mode..." << std::endl;
    } else if(command==5) {
      withData=false;
      calcRate=true;
      std::cout << std::endl
		<< "Calling AZURE in reaction rate mode..." << std::endl;      
    }
    configure.performFit=performFit;
    configure.performError=performError;
    configure.chiVariance=chiVariance;
    configure.withData=withData;
    configure.oldParameters=oldParameters;
    configure.isEC=isEC;
    configure.oldECFile=oldECFile;
    configure.calcRate=calcRate;
    configure.rateEntrancePair=rateEntrancePair;
    configure.rateExitPair=rateExitPair;
    configure.rateMinTemp=rateMinTemp;
    configure.rateMaxTemp=rateMaxTemp;
    configure.rateTempStep=rateTempStep;

    AZUREMain azureMain(configure);
    azureMain();
  }

  std::cout << std::endl
	    << "Thanks for using AZURE." << std::endl;
  if(useReadline) write_history("./.azure_history");
  return 0;
}
