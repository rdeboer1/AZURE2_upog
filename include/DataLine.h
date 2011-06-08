#ifndef DATALINE_H
#define DATALINE_H

#include <fstream>

class DataLine {
 public:
  DataLine(std::ifstream &stream) {
    stream >> energy_ >> angle_ >> crossSection_ >> error_;
  };
  double angle() const {return angle_;};
  double energy() const {return energy_;};
  double crossSection() const {return crossSection_;};
  double error() const {return error_;};
 private:
  double angle_;
  double energy_;
  double crossSection_;
  double error_;
};

#endif
