#include "ECLine.h"
  
ECLine ReadECLine(std::ifstream &stream)
{
  ECLine a;
  std::string dummy;

  stream >> a.isdc >> a.entrancekey >> a.exitkey >> a.jimin >> a.jimax 
	 >> a.limin >> a.limax >> a.maxmult;
  getline(stream,dummy);

  a.jimin=a.jimin/2.;
  a.jimax=a.jimax/2;
  
  return a;
}
