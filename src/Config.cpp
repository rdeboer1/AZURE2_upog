#include "Config.h"
#ifndef NO_STAT
#include <sys/stat.h>
#endif
#include <iostream>

int ReadConfigFile(Config &configure) {
  std::string dummy; std::string temp;
  std::ifstream in(configure.configfile.c_str());
  if(!in) return -1;
  in >> temp;getline(in,dummy);
  if(temp=="true") configure.isAMatrix=true;
  else configure.isAMatrix=false;
  in >> configure.nucfile;getline(in,dummy);
  in >> configure.ecfile;getline(in,dummy);
  in >> configure.segfile;getline(in,dummy);
  in >> configure.extrapfile;getline(in,dummy);
  in >> configure.targeteffectsfile;getline(in,dummy);
  in >> configure.outputdir;getline(in,dummy);
  in >> configure.checkdir;getline(in,dummy);
  in >> configure.checknucleus;getline(in,dummy);
  in >> configure.checkboundcon;getline(in,dummy);
  in >> configure.checkdata;getline(in,dummy);
  in >> configure.checkpene;getline(in,dummy);
  in >> configure.checklegpoly;getline(in,dummy);
  in >> configure.checkcoulamp;getline(in,dummy);
  in >> configure.checkpathways;getline(in,dummy);
  in >> configure.checkangdists;getline(in,dummy);
  in.close();
  return 0;
}

#ifndef NO_STAT
int CheckForInputFiles(Config &configure) {
  struct stat buffer;
  if(stat(configure.nucfile.c_str(),&buffer) != 0) {
    std::cout << "Could not find nuclear file: " << configure.nucfile << ". Check that it exists." << std::endl;
    return -1;
  }
  if(stat(configure.ecfile.c_str(),&buffer) != 0) {
    std::cout << "Could not find external capture file: " << configure.ecfile << ". Check that it exists." << std::endl;
    return -1;
  }
  if(stat(configure.segfile.c_str(),&buffer) != 0) {
    std::cout << "Could not find data segments file: " << configure.segfile << ". Check that it exists." << std::endl;
    return -1;
  }
  if(stat(configure.extrapfile.c_str(),&buffer) != 0) {
    std::cout << "Could not find extrapolation segments file: " << configure.extrapfile << ". Check that it exists." << std::endl;
    return -1;
  }
  if(stat(configure.targeteffectsfile.c_str(),&buffer) != 0) {
    std::cout << "Could not find target effects file: " << configure.targeteffectsfile << ". Check that it exists." << std::endl;
    return -1;
  }
  if(stat(configure.outputdir.c_str(),&buffer) != 0) {
    std::cout << "Could not find output directory: " << configure.outputdir << ". Check that it exists." << std::endl;
    return -1;
  }
  if(stat(configure.checkdir.c_str(),&buffer) != 0) {
    std::cout << "Could not find checks directory: " << configure.checkdir << ". Check that it exists." << std::endl;
    return -1;
  }
  return 0;
}
#endif
