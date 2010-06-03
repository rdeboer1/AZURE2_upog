#ifndef ECINTEGRAL_H
#define ECINTEGRAL_H

#include "CoulFunc.h"
#include "WhitFunc.h"

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
  ECIntegral(PPair *pPair) {
    coulfunc_ = new CoulFunc(pPair);
    whitfunc_ = new WhitFunc(pPair);
    chanrad_ = pPair->GetChRad();
    total_sep_e_=pPair->GetSepE()+pPair->GetExE();
  };
  /*!
   * The CoulFunc and WhitFunc objects are destroyed with the object.
   */
  ~ECIntegral() {
    delete coulfunc_;
    delete whitfunc_;
  };
  /*!
   * Overloaded operator to make the class instance callable as a function.  The intial and final orbital
   * angular momentum, the gamma multipolarity, and the incoming energy and final state energy in the 
   * compound system are the dependent variables. The function returns an ECIntResult structure.
   */  
  ECIntResult operator()(int li, int lf, int multL, double inEnergy, double levelEnergy) const {
    return gsl_ec_integral(coulfunction(),whitfunction(),li,lf,multL,
			   inEnergy-TotalSepE(),fabs(levelEnergy-TotalSepE()),ChanRad());
  };
  /*!
   * Returns a pointer to the Coulomb function.
   */
  CoulFunc *coulfunction() const {return coulfunc_;};
  /*!
   * Returns a pointer to the Whittaker function.
   */
  WhitFunc *whitfunction() const {return whitfunc_;};
  /*! 
   * Returns the channel radius for the particle pair.
   */
  double ChanRad() const {return chanrad_;};
  /*! 
   * Returns the total seperation energy for the particle pair.
   */
  double TotalSepE() const {return total_sep_e_;};
 private:
  CoulFunc *coulfunc_;
  WhitFunc *whitfunc_;
  double chanrad_;
  double total_sep_e_ ;
};




#endif
