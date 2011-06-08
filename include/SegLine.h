#ifndef SEGLINE_H
#define SEGLINE_H

#include <iostream>
#include <string>

class SegLine {
 public:
  SegLine(std::istream &stream) {
    stream >> isActive_ >>  entranceKey_ >> exitKey_ >> minE_ >> maxE_ >> minA_ >> maxA_ >> isDiff_;
    if(isDiff_==2) stream >> phaseJ_ >> phaseL_;
    stream >> dataNorm_ >> varyNorm_ >> dataFile_;
  };
  int isActive() const {return isActive_;};
  int entranceKey() const {return entranceKey_;};
  int exitKey() const {return exitKey_;};
  double minE() const {return minE_;};
  double maxE() const {return maxE_;};
  double minA() const {return minA_;};
  double maxA() const {return maxA_;};
  int isDiff() const {return isDiff_;};
  std::string dataFile() const {return dataFile_;};
  double dataNorm() const {return dataNorm_;};
  int varyNorm() const {return varyNorm_;};
  double phaseJ() const {return phaseJ_;};
  int phaseL() const {return phaseL_;};
 private:
  int isActive_;
  int entranceKey_;
  int exitKey_;
  double minE_;
  double maxE_;
  double minA_;
  double maxA_;
  int isDiff_;
  std::string dataFile_;
  double dataNorm_;
  int varyNorm_;
  double phaseJ_;
  int phaseL_;
};

#endif
