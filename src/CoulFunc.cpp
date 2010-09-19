#include "CoulFunc.h"
#include "cwfcomp.H"
#include "PPair.h"

extern double gsl_PEshift_function_dE(CoulFunc*,int,double,double);

/*! 
 * The CoulFunc object is created with reference to a PPair object.
 */

CoulFunc::CoulFunc(PPair *pPair) {
  z1_=pPair->GetZ(1);
  z2_=pPair->GetZ(2);
  redmass_=(double)pPair->GetRedMass();
  lLast_=0;
  radiusLast_=0.0;
  energyLast_=0.0;
  coulLast_.F=0.0;
  coulLast_.dF=0.0;
  coulLast_.G=0.0;
  coulLast_.dG=0.0;
}

/*!
 * Returns the atomic number of the first particle in the pair.
 */

int CoulFunc::z1() const {
  return z1_;
}

/*!
 * Returns the atomic number of the second particle in the pair.
 */

int CoulFunc::z2() const {
  return z2_;
}
 
/*!
 * Returns the reduced mass of the particle pair.
 */

double CoulFunc::redmass() const {
  return redmass_;
}

/*!
 * Returns the last orbital angular momentum value at which the Coulomb 
 * functions were calculated.
 */

int CoulFunc::lLast() const {
  return lLast_;
}

/*!
 * Returns the last radius value at which the Coulomb 
 * functions were calculated.
 */
 
double CoulFunc::radiusLast() const {
  return radiusLast_;
}

/*!
 * Returns the last energy value at which the Coulomb 
 * functions were calculated.
 */

double CoulFunc::energyLast() const {
  return energyLast_;
}

/*!
 * Returns the last Coulomb functions which were calculated.
 */

struct CoulWaves CoulFunc::coulLast() const {
  return coulLast_;
}

/*!
 * Sets the last calculated Coulomb waves and the values for which they were calculated.
 */
 
void CoulFunc::setLast(int lLast, double rLast, double eLast, CoulWaves coulLast) {
  lLast_=lLast;
  radiusLast_=rLast;
  energyLast_=eLast;
  coulLast_.F=coulLast.F;
  coulLast_.dF=coulLast.dF;
  coulLast_.G=coulLast.G;
  coulLast_.dG=coulLast.dG;
}

/*!
 * The parenthesis operator is defined to make the class instance callable as a function.  The orbital
 * angular momentum, radius, and energy in the center of mass system are the dependent variables.
 * The function returns the Coulomb waves.
 */

CoulWaves CoulFunc::operator()(int l,double radius,double energy) {
  struct CoulWaves result={0.0,0.0,0.0,0.0};
  if(l==lLast()&&radius==radiusLast()&&energy==energyLast()) {
    result=coulLast();
  } else {
    std::complex<double> eta(sqrt(uconv/2.)*fstruc*z1()*z2()*
			     sqrt(redmass()/energy),0.);
    std::complex<double> rho(sqrt(2.*uconv)/hbarc*radius*
			     sqrt(redmass()*energy),0.);
    std::complex<double> lValue( (double) l, 0.);
    Coulomb_wave_functions coul(true,lValue,eta);
    std::complex<double> c_F, c_dF, c_G, c_dG;
    coul.F_dF(rho,c_F,c_dF);
    coul.G_dG(rho,c_G,c_dG);
    struct CoulWaves newResult={real(c_F),real(c_dF),real(c_G),real(c_dG)};
    setLast(l,radius,energy,newResult);
    result=newResult;
  }
  return result;
}

/*!
 * Returns the penetrability as a function of orbital
 * angular momentum, radius, and energy in the center of mass system.
 */

double CoulFunc::Penetrability(int l,double radius,double energy)  {
  struct CoulWaves coul=this->operator()(l,radius,energy);
  double rho=sqrt(2.*uconv)/hbarc*radius*sqrt(redmass()*energy);
  return rho/(pow(coul.F,2.0)+pow(coul.G,2.0));
}

/*!
 * Returns the positive energy shift function a function of orbital
 * angular momentum, radius, and energy in the center of mass system.
 */

double CoulFunc::PEShift(int l,double radius,double energy)  {
  struct CoulWaves coul=this->operator()(l,radius,energy);
  double rho=sqrt(2.*uconv)/hbarc*radius*sqrt(redmass()*energy);
  return rho/(pow(coul.F,2.0)+pow(coul.G,2.0))*
    (coul.F*coul.dF+coul.G*coul.dG);
}

/*!
 * Returns the energy derivative of the shift function a function of orbital
 * angular momentum, radius, and energy in the center of mass system.
 */

double CoulFunc::PEShift_dE(int l,double radius,double energy)  {
  return gsl_PEshift_function_dE(this,l,radius,energy);
}

