#ifndef ECLINE_H
#define ECLINE_H

#include<iostream>

class ECLine {
 public:
  ECLine(std::istream &stream) {
    std::string dummy;

    stream >> isEC_ >> exitKey_ >> jiMin_ >> jiMax_  >> multMask_;getline(stream,dummy);
   
    jiMin_/=2.;
    jiMax_/=2.;
  };
  int isEC() const {return isEC_;};
  int exitKey() const {return exitKey_;};
  double jiMin() const {return jiMin_;};
  double jiMax() const {return jiMax_;};
  unsigned char multMask() const {return multMask_;};
 private:
  int isEC_;
  int exitKey_;
  double jiMin_;
  double jiMax_;
  unsigned char multMask_;
};

#endif
