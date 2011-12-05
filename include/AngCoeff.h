#ifndef ANGCOEFF_H
#define ANGCOEFF_H

///A container class for angular coupling coefficient functions.

/*!
 * The AngCoeff class serves as a container class for the angular momentum coupling 
 * coefficients.  
 */

class AngCoeff {
 public:
  static double ClebGord(double,double,double,double,double,double);
  static double Racah(double,double,double,double,double,double);
};

#endif
