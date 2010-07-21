#ifndef SHIFTFUNC_H
#define SHIFTFUNC_H

#include "PPair.h"
#include "WhitFunc.h"

class ShftFunc;

extern double gsl_shift_function(int,double,double,double,WhitFunc*);
extern double gsl_shift_function_dE(int,double,const ShftFunc*);

///A function class for negative energy shift functions.

/*!
 * A shift function for negative energy channels is calculated as 
 * \f$ S=\rho \frac{W_c'(k\rho)}{W_c(k\rho)} \f$, where the prime indicates
 * the derivative with respect to \f$ \rho \f$. The AZURE function class ShftFunc
 * uses the GSL package to calculates the numerical derivative.
 */

class ShftFunc {
 public:
  /*!
   * The ShftFunc object is created with reference to a particle pair.
   */
  ShftFunc(PPair* pPair) {
    z1_=pPair->GetZ(1);
    z2_=pPair->GetZ(2);
    redmass_=(double) pPair->GetRedMass();
    totalSepE_=pPair->GetSepE()+pPair->GetExE();
    radius_=(double) pPair->GetChRad();
    whit_= new WhitFunc(pPair);
  };
  /*!
   *
   */
  ~ShftFunc() {
    delete whit_;
  };
  /*!
   * Returns the atomic number of the first particle in the pair.
   */
  int z1() const {return z1_;};
  /*!
   * Returns the atomic number of the second particle in the pair.
   */
  int z2() const {return z2_;};
  /*!
   * Returns the reduced mass of the particle pair.
   */
  double redmass() const {return redmass_;};
  /*!
   * Returns the total seperation energy of the particle pair.
   */
  double totalSepE() const {return totalSepE_;};
  /*!
   * Returns the channel radius of the particle pair.
   */
  double radius() const {return radius_;};
  /*!
   * The parenthesis operator is defined to make the class instance callable as a function.  The orbital
   * angular momentum and energy in the compound system are the dependent variables.
   * The function returns the value of the shift function.
   */
  double operator()(int l,double energy) const {
    return gsl_shift_function(l,energy,totalSepE(),radius(),whit());
  };
  /*!
   * Returns the energy derivative of the shift function at the specified orbital 
   * angular momentum and energy in the compound system.
   */
  double EnergyDerivative(int l,double energy) const {
    return gsl_shift_function_dE(l,energy,this);
  };
  /*!
   * Returns a pointer to the Whittaker functions used in the calculation of the shift functions.
   */
  WhitFunc *whit() const {
    return whit_;
  };
 private:
  int z1_;
  int z2_;
  double redmass_;
  double totalSepE_;
  double radius_;
  WhitFunc *whit_;
};

#endif
