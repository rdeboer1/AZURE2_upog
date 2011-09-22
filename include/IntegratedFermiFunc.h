#ifndef INTEGRATEDFERMIFUNC_H
#define INTEGRATEDFERMIFUNC_H

class IntegratedFermiFunc {
 public:
  IntegratedFermiFunc(double V0 = 0.);
  double operator()(double,double,double);
 private:
  static const double alpha_=1./137.036;
  static const double pi_=3.14159;
  static const double electronMass_=0.51099891;
  static const double hbarc_=197.327;
  static double Integrand(double, void*);  
  typedef struct Params_ {
    double gamma0;
    double Z;
    double radius;
    double W0;
    double GammaDenom2;
    double V0;
  } Params_;
  double V0_;
};

#endif
