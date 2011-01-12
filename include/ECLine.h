#ifndef ECLINE_H
#define ECLINE_H

#include<iostream>

struct ECLine {
  int isdc;
  int exitkey;
  double jimin;
  double jimax;
  unsigned char multMask;
};

ECLine ReadECLine(std::istream &stream);

#endif
