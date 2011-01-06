#ifndef SEGLINE_H
#define SEGLINE_H

#include <iostream>
#include <string>

struct SegLine {
  int isActive;
  int entrancekey;
  int exitkey;
  double min_e;
  double max_e;
  double min_a;
  double max_a;
  int diff;
  std::string datafile;
  double dataNorm;
  int varyNorm;
  double j;
  int l;
};

SegLine ReadSegLine(std::istream &stream);
#endif
