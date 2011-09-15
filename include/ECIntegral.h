#ifndef ECINTEGRAL_H
#define ECINTEGRAL_H

#include "CoulFunc.h"
#include "WhitFunc.h"
#include "Config.h"

///Return structure for ECIntegral function class

/*!
 * The ECintResult structure contains the result of the integral 
 * of regular and irregular Coulomb functions multiplied by Whittaker 
 * functions and \f$ r^L \f$ for EL capture transitions 
 * (term not present for M1 transitions). Negative energy initial 
 * channels are also considered, where the Coulomb function is 
 * replaced by a Whittaker function. In such a case, the result 
 * is returned in the GW attribute.
 */

struct ECIntResult {
  ///Integral of regular Coulomb solution, Whittaker function, and operator.
  double FW;
  ///Integral of irregular Coulomb solution (or Whittaker function for negative energies), Whittaker function, and operator.
  double GW;
};

extern struct ECIntResult gsl_ec_integral(CoulFunc*,WhitFunc*,int,int,int,
		double,double,double);

///A function class to calculate external capture integrals

/*!
 * The ECIntegral function class calculates external capture integrals for 
 * both positive and negative energy channels.  The results are returned as 
 * an ECIntResult structure.
 */

class ECIntegral {
 public:
  /*!
   * The ECIntegral object is created with reference to a PPair object. 
   * The PPair object is used to create new instances of the CoulFunc and 
   * WhitFunc objects.  
   */
  ECIntegral(PPair *pPair, const Config& configure) {
    coulfunc_ = new CoulFunc(pPair,!!(configure.paramMask&Config::USE_GSL_COULOMB_FUNC));
    whitfunc_ = new WhitFunc(pPair);
    pair_ = pPair;
  };
  /*!
   * The CoulFunc and WhitFunc objects are destroyed with the object.
   */
  ~ECIntegral() {
    delete coulfunc_;
    delete whitfunc_;
  };
  complex operator()(int,int,double,double,double,double,int,char,double,double,bool) const;
  /*!
   * Returns a pointer to the Coulomb function.
   */
  CoulFunc *coulfunction() const {return coulfunc_;};
  /*!
   * Returns a pointer to the Whittaker function.
   */
  WhitFunc *whitfunction() const {return whitfunc_;};
  /*!
   * Returns the particle pair pointer.
   */
  PPair *pair() const {return pair_;};
 private:
  CoulFunc *coulfunc_;
  WhitFunc *whitfunc_;
  PPair *pair_;
};




#endif
