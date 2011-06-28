#include "Config.h"
#ifndef NO_STAT
#include <sys/stat.h>
#endif
#include <iostream>

int ReadConfigFile(Config &configure) {
  std::string dummy; std::string temp;
  std::ifstream in(configure.configfile.c_str());
  if(!in) return -1;
  std::string line="";
  while(line!="<config>"&&!in.eof()) getline(in,line);
  if(line!="<config>") return -1;
  in >> temp;getline(in,dummy);
  if(temp=="true") configure.mask |= USE_AMATRIX;
  else configure.mask &= ~USE_AMATRIX;
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
  line="";
  while(line!="</config>"&&!in.eof()) getline(in,line);
  if(line!="</config>") return -1;
  in.close();
  return 0;
}

#ifndef NO_STAT
int CheckForInputFiles(Config &configure) {
  struct stat buffer;
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
