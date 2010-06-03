#include "DataLine.h"

DataLine ReadDataLine(std::ifstream &stream) {
  DataLine a;
  stream >> a.energy >> a.angle >> a.cross >> a.error;
  return a;
}
