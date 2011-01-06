#ifndef ECLINE_H
#define ECLINE_H

#include<iostream>

struct ECLine {
  int isdc;
  int entrancekey;
  int exitkey;
  double jimin;
  double jimax;
  int limin;
  int limax;
  int maxmult;
};

ECLine ReadECLine(std::istream &stream);

#endif
