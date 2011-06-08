#ifndef NUCLINE_H
#define NUCLINE_H

#include <iostream>

class NucLine {
 public:
  NucLine(std::istream &stream) {
    stream >> levelJ_ >> levelPi_ >> levelE_ >> levelFix_ >> aa_ >> ir_ 
	   >> s_ >> l_ >> levelID_ >> isActive_ >> channelFix_ >> gamma_ >> j1_ >> pi1_
	   >> j2_ >> pi2_ >> e2_ >> m1_ >> m2_ >> z1_ >> z2_
	   >> entranceSepE_ >> sepE_ >> j3_ >> pi3_ >> e3_ 
	   >> pType_ >> chRad_ >> g1_ >> g2_;
    s_/=2.;
    l_/=2;
  };
  double levelJ() const {return levelJ_;};
  int levelPi() const {return levelPi_;};
  double levelE() const {return levelE_;};
  int levelFix() const {return levelFix_;};
  int aa() const {return aa_;};
  int ir() const {return ir_;};
  double s() const {return s_;};
  int l() const {return l_;};
  int levelID() const {return levelID_;};
  int isActive() const {return isActive_;};
  int channelFix() const {return channelFix_;};
  double gamma() const {return gamma_;};
  double j1() const {return j1_;};
  int pi1() const {return pi1_;};
  double j2() const {return j2_;};
  int pi2() const {return pi2_;};
  double e2() const {return e2_;};
  double m1() const {return m1_;};
  double m2() const {return m2_;};
  int z1() const {return z1_;};
  int z2() const {return z2_;};
  double entranceSepE() const {return entranceSepE_;};
  double sepE() const {return sepE_;};
  int j3() const {return j3_;};
  int pi3() const {return pi3_;};
  double e3() const {return e3_;};
  int pType() const {return pType_;};
  double chRad() const {return chRad_;};
  double g1() const {return g1_;};
  double g2() const {return g2_;};
 private:
  double levelJ_;
  int levelPi_;
  double levelE_;
  int levelFix_;
  int aa_;
  int ir_;
  double s_;
  int l_;
  int levelID_;
  int isActive_;
  int channelFix_;
  double gamma_;
  double j1_;
  int pi1_;
  double j2_;
  int pi2_;
  double e2_;
  double m1_;
  double m2_;
  int z1_;
  int z2_;
  double entranceSepE_;
  double sepE_;
  int j3_;
  int pi3_;
  double e3_;
  int pType_;
  double chRad_;
  double g1_;
  double g2_;
};

#endif
