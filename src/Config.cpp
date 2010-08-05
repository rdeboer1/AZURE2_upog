#include "Config.h"

int ReadConfigFile(Config &a) {
  std::string dummy; std::string temp;
  std::ifstream in(a.configfile.c_str());
  if(!in) return -1;
  in >> temp;getline(in,dummy);
  if(temp=="true") a.isAMatrix=true;
  else a.isAMatrix=false;
  in >> a.nucfile;getline(in,dummy);
  in >> a.ecfile;getline(in,dummy);
  in >> a.segfile;getline(in,dummy);
  in >> a.extrapfile;getline(in,dummy);
  in >> a.targeteffectsfile;getline(in,dummy);
  in >> a.outputdir;getline(in,dummy);
  in >> a.checkdir;getline(in,dummy);
  in >> a.checknucleus;getline(in,dummy);
  in >> a.checkboundcon;getline(in,dummy);
  in >> a.checkdata;getline(in,dummy);
  in >> a.checkpene;getline(in,dummy);
  in >> a.checklegpoly;getline(in,dummy);
  in >> a.checkcoulamp;getline(in,dummy);
  in >> a.checkpathways;getline(in,dummy);
  in >> a.checkangdists;getline(in,dummy);
  in.close();
  return 0;
}
