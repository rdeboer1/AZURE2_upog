#include "AZUREMain.h"
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
 * data relavent to nuclear astrophysics.  This release of AZURE has been 
 * completely rewritten in object-oriented C++.  The documentation contained 
 * within these pages should serve as an introduction to the object structure of
 * AZURE Version 2.0.
 */

int main(int argc,char *argv[]){

  if(argc<2) {
    std::cout << "Too few arguments.  Configuration file must be specified." << std::endl
	      << "\tSyntax: azure2 configfile" << std::endl;
    return -1;
  }
  read_history("./.azure_history");
  std::cout << std::endl
	    << "O--------------------------O----------------------------O" << std::endl
	    << "| #### #### #  # ###  #### | Version 2.0                |" << std::endl
	    << "| #  #    # #  # #  # #    | Object Oriented C++        |" << std::endl
	    << "| ####   #  #  # ###  ##   |                            |" << std::endl
	    << "| #  #  #   #  # # #  #    | Ethan Uberseder            |" << std::endl
	    << "| #  # ####  ##  #  # #### | University of Notre Dame   |" << std::endl
	    << "O--------------------------O----------------------------O" << std::endl
	    << std::endl;
  
  //Read Runtime file
  Config configure;
  configure.configfile=argv[1];
  if(ReadConfigFile(configure)==-1) {
    std::cout << "Could not open " << configure.configfile << ".  Check that file exists." 
	      << std::endl;
    return -1;
  }

  int command=0;

  std::cout << "Please select from the following options: " << std::endl
	    << "\t1. Data Fit" << std::endl
	    << "\t2. Data Calculate" << std::endl
	    << "\t3. Extrapolate (no data)" << std::endl
	    << "\t4. Reaction Rate" << std::endl
	    << "\t5. Exit" << std::endl;

  while(command!=1&&command!=2&&command!=3&&command!=4&&command!=5) {
    std::cout << "azure2: ";
    while(!(std::cin >> command)) {
      std::cin.clear();
      std::cin.ignore(1000,'\n');
      std::cout << "Please enter an integer." << std::endl;
      std::cout << "azure2: ";
    }
    if(command!=1&&command!=2&&command!=3&&command!=4) {
      std::cout << "Invalid option.  Please try again."
		<< std::endl;
    }
  }
  
  if(command!=5) {
    int rateEntrancePair=0;
    int rateExitPair=0;
    double rateMinTemp=0.0;
    double rateMaxTemp=0.0;
    double rateTempStep=0.0;
    bool performFit=false;
    bool oldParameters=false;
    bool validInfile=false;
    bool withData=true;
    bool calcRate=false;
    std::string inFile;
    std::ifstream in;
    std::cout << std::endl;
    std::cin.ignore(1000,'\n');
    while(!validInfile) {
      char *line = readline("External Parameter File (leave blank for new file): ");
      inFile=line;
      if(line && *line) add_history(line);
      free(line);
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
      if(!validInfile) std::cout << "Cannot Read From " << inFile << ". Please reenter file." << std::endl;
    }
    bool isEC=false;
    std::vector<SegPairs> segPairs;
    if(command==1||command==2||command==3) {
      if(command==1||command==2) in.open(configure.segfile.c_str());
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
    } else if(command==4) {
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
    if(isEC&&command!=4) {
      std::cout << std::endl;
      while(!validInfile) {
	char *line = readline("External Capture Amplitude File (leave blank for new file): ");
	inFile=line;
	if(line && *line) add_history(line);
	free(line);
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
	if(!validInfile) std::cout << "Cannot Read From " << inFile << ". Please reenter file." << std::endl;
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
      withData=false;
      calcRate=true;
      std::cout << std::endl
		<< "Calling AZURE in reaction rate mode..." << std::endl;      
    }
    configure.performFit=performFit;
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
  write_history("./.azure_history");
  return 0;
}
