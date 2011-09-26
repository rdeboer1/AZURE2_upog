#ifndef INTEGRATEDFERMIFUNC_H
#define INTEGRATEDFERMIFUNC_H

///A function class to calculate the integrated Fermi function for beta decay

/*!
 * This function class calculates the integrated Fermi function for beta decay channels.
 * The integrand is taken directly from Konopinski and Rose.  While screening potentials
 * can be added, by default the screening potential is set to zero.  This function
 * class should be valid for either electron or positron emission.  
 */

class IntegratedFermiFunc {
 public:
  IntegratedFermiFunc(int,double V0 = 0.);
  double operator()(double,double,double);
 private:
  static const double alpha_=1./137.036;
  static const double pi_=3.14159;
  static const double electronMass_=0.51099891;
  static const double hbarc_=197.327;
  static double Integrand(double, void*);  
  typedef struct Params_ {
    int charge;
    double gamma0;
    double Z;
    double radius;
    double W0;
    double GammaDenom2;
    double V0;
  } Params_;
  int charge_;
  double V0_;
};

#endif
