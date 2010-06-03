#ifndef DATALINE_H
#define DATALINE_H

#include <fstream>

struct DataLine {
  double angle;
  double energy;
  double cross;
  double error;
};

DataLine ReadDataLine(std::ifstream &stream);

#endif
