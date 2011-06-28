#include "Config.h"
#ifndef NO_STAT
#include <sys/stat.h>
#endif
#include <iostream>

Config::Config(std::ostream& stream) : outStream(stream) {
  chiVariance=1.0;
  screenCheckMask=0;
  fileCheckMask=0;
  paramMask=0;
  paramMask |= (TRANSFORM_PARAMETERS|CALCULATE_WITH_DATA);
}

int Config::ReadConfigFile() {
  std::string dummy; std::string temp;
  std::ifstream in(configfile.c_str());
  if(!in) return -1;
  std::string line="";
  while(line!="<config>"&&!in.eof()) getline(in,line);
  if(line!="<config>") return -1;
  in >> temp;getline(in,dummy);
  if(temp=="true") paramMask |= USE_AMATRIX;
  in >> outputdir;getline(in,dummy);
  in >> checkdir;getline(in,dummy);
  in >> temp;getline(in,dummy);
  if(temp=="screen") screenCheckMask |= CHECK_COMPOUND_NUCLEUS;
  else if(temp=="file") fileCheckMask |= CHECK_COMPOUND_NUCLEUS;
  in >> temp;getline(in,dummy);
  if(temp=="screen") screenCheckMask |= CHECK_BOUNDARY_CONDITIONS;
  else if(temp=="file") fileCheckMask |= CHECK_BOUNDARY_CONDITIONS;
  in >> temp;getline(in,dummy);
  if(temp=="screen") screenCheckMask |= CHECK_DATA;
  else if(temp=="file") fileCheckMask |= CHECK_DATA;
  in >> temp;getline(in,dummy);
  if(temp=="screen") screenCheckMask |= CHECK_ENERGY_DEP;
  else if(temp=="file") fileCheckMask |= CHECK_ENERGY_DEP;
  in >> temp;getline(in,dummy);
  if(temp=="screen") screenCheckMask |= CHECK_LEGENDRE;
  else if(temp=="file") fileCheckMask |= CHECK_LEGENDRE;
  in >> temp;getline(in,dummy);
  if(temp=="screen") screenCheckMask |= CHECK_COUL_AMPLITUDES;
  else if(temp=="file") fileCheckMask |= CHECK_COUL_AMPLITUDES;
  in >> temp;getline(in,dummy);
  if(temp=="screen") screenCheckMask |= CHECK_PATHWAYS;
  else if(temp=="file") fileCheckMask |= CHECK_PATHWAYS;
  in >> temp;getline(in,dummy);
  if(temp=="screen") screenCheckMask |= CHECK_ANGULAR_DISTS;
  else if(temp=="file") fileCheckMask |= CHECK_ANGULAR_DISTS;
  line="";
  while(line!="</config>"&&!in.eof()) getline(in,line);
  if(line!="</config>") return -1;
  in.close();
  return 0;
}

#ifndef NO_STAT
int Config::CheckForInputFiles() {
  struct stat buffer;
  if(stat(outputdir.c_str(),&buffer) != 0) {
    outStream << "Could not find output directory: " << outputdir << ". Check that it exists." << std::endl;
    return -1;
  }
  if(stat(checkdir.c_str(),&buffer) != 0) {
    outStream << "Could not find checks directory: " << checkdir << ". Check that it exists." << std::endl;
    return -1;
  }
  return 0;
}
#endif
