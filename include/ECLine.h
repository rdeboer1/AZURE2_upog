#ifndef ECLINE_H
#define ECLINE_H

#include<iostream>

///A class to read and store a line from the external capture input file.

/*!
 * The ECLine class reads ands stores a formatted line from the 
 * external capture input file.
 */

class ECLine {
 public:
  /*!
   * Constructor fills the ECLine object from an input stream.
   */
  ECLine(std::istream &stream) {
    std::string dummy;

    stream >> isActive_ >> exitKey_ >> jiMin_ >> jiMax_  >> multMask_;getline(stream,dummy);
   
    jiMin_/=2.;
    jiMax_/=2.;
  };
  /*!
   * Returns non-zero if the line is to be included in the calculation.
   */
  int isActive() const {return isActive_;};
  /*!
   * Returns the particle pair key corresponding to the final state 
   * and a gamma.
   */
  int exitKey() const {return exitKey_;};
  /*!
   * Returns the spin of the lowest initial partial wave to be 
   * considered in the calculation.
   */
  double jiMin() const {return jiMin_;};
  /*!
   * Returns the spin of the highest initial partial wave to be 
   * considered in the calculation.
   */
  double jiMax() const {return jiMax_;};
  /*!
   * Returns a bitmask with the encoded multipolarities
   *  to be considered in the calculation.
   */
  unsigned char multMask() const {return multMask_;};
 private:
  int isActive_;
  int exitKey_;
  double jiMin_;
  double jiMax_;
  unsigned char multMask_;
};

#endif
