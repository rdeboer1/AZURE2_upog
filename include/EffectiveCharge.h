#ifndef EFFECTIVECHARGE_H
#define EFFECTIVECHARGE_H

class PPair;

class EffectiveCharge {
 public:
  EffectiveCharge(PPair*,double, int);
  double operator()(double);
 private:
  static double Integrand(double,void*);
  int z1_;
  int z2_;
  int L_;
  double m1_;
  double m2_;
  double energy_;
};

#endif
