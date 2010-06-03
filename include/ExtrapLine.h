#ifndef EXTRAPLINE_H
#define EXTRAPLINE_H

#include <fstream>
#include <string>

struct ExtrapLine {
  int isActive;
  int entrancekey;
  int exitkey;
  double min_e;
  double max_e;
  double min_a;
  double max_a;
  double e_step;
  double a_step;
  int diff;
  double j;
  int l;
};

ExtrapLine ReadExtrapLine(std::ifstream &stream);
#endif
