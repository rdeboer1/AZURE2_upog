#ifndef EXTRAPLINE_H
#define EXTRAPLINE_H

#include <iostream>
#include <string>

class ExtrapLine {
 public:
  ExtrapLine(std::istream &stream) {
    stream >> isActive_ >> entranceKey_ >> exitKey_ >> minE_ 
	   >> maxE_ >> eStep_ >> minA_ >> maxA_ >> aStep_ >> isDiff_;
    if(isDiff_==2) stream >> phaseJ_ >> phaseL_;
  };
  int isActive() const {return isActive_;};
  int entranceKey() const {return entranceKey_;};
  int exitKey() const {return exitKey_;};
  double minE() const {return minE_;};
  double maxE() const {return maxE_;};
  double minA() const {return minA_;};
  double maxA() const {return maxA_;};
  double eStep() const {return eStep_;};
  double aStep() const {return aStep_;};
  int isDiff() const {return isDiff_;};
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
  double eStep_;
  double aStep_;
  int isDiff_;
  double phaseJ_;
  int phaseL_;
};

#endif
