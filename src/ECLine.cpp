#include "ECLine.h"
  
ECLine ReadECLine(std::istream &stream)
{
  ECLine a;
  std::string dummy;

  stream >> a.isdc >> a.exitkey >> a.jimin >> a.jimax  >> a.multMask;
  getline(stream,dummy);

  a.jimin=a.jimin/2.;
  a.jimax=a.jimax/2;
  
  return a;
}
