#ifndef COULFUNC_H
#define COULFUNC_H

#include <gsl/gsl_sf_coulomb.h>
#include "Constants.h"
#include "PPair.h"

class CoulFunc;

extern double gsl_PEshift_function_dE(CoulFunc*,int,double,double);

///The return structure of the CoulFunc function class.

/*! 
 * The CoulWaves structure contains both the irregular and regular solutions to the
 * Coulomb equation, as well as their derivatives with respect to \f$ \rho \f$.
 */

struct CoulWaves {
  ///Regular solution
  double F;
  ///Derivative of regular solution with respect to \f$ \rho \f$
  double dF;
  ///Irregular solution
  double G;
  ///Derivative of irregular solution with respect to \f$ \rho \f$
  double dG;
};

///A function class to calculate Coulomb functions for positive energy channels

/*! 
 * The CoulFunc function class calculates the solutions to the Coulomb equation, 
 * as well as other useful quantities such as shift functions and their energy
 * derivative and penetrabilities. 
 */

class CoulFunc {
  public:
   /*! 
    * The CoulFunc object is created with reference to a PPair object.
    */
    CoulFunc(PPair *pPair) {
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
    };
  /*!
   * Returns the atomic number of the first particle in the pair.
   */
    int z1() const {
      return z1_;
    };
  /*!
   * Returns the atomic number of the second particle in the pair.
   */
    int z2() const {
      return z2_;
    };
  /*!
   * Returns the reduced mass of the particle pair.
   */
    double redmass() const {
      return redmass_;
    };
  /*!
   * Returns the last orbital angular momentum value at which the Coulomb 
   * functions were calculated.
   */
    int lLast() const {
      return lLast_;
    };
  /*!
   * Returns the last radius value at which the Coulomb 
   * functions were calculated.
   */
    double radiusLast() const {
      return radiusLast_;
    };
  /*!
   * Returns the last energy value at which the Coulomb 
   * functions were calculated.
   */
    double energyLast() const {
      return energyLast_;
    };
  /*!
   * Returns the last Coulomb functions which were calculated.
   */
    struct CoulWaves coulLast() const {
      return coulLast_;
    };
  /*!
   * Sets the last calculated Coulomb waves and the values for which they were calculated.
   */
    void setLast(int lLast, double rLast, double eLast, CoulWaves coulLast) {
      lLast_=lLast;
      radiusLast_=rLast;
      energyLast_=eLast;
      coulLast_.F=coulLast.F;
      coulLast_.dF=coulLast.dF;
      coulLast_.G=coulLast.G;
      coulLast_.dG=coulLast.dG;
    };
  /*!
   * The parenthesis operator is defined to make the class instance callable as a function.  The orbital
   * angular momentum, radius, and energy in the center of mass system are the dependent variables.
   * The function returns the Coulomb waves.
   */
    struct CoulWaves operator()(int l,double radius,double energy)  {
      struct CoulWaves result={0.0,0.0,0.0,0.0};
      if(l==lLast()&&radius==radiusLast()&&energy==energyLast()) {
	result=coulLast();
      } else {

	double eta=sqrt(uconv/2.)*fstruc*z1()*z2()*
	  sqrt(redmass()/energy);
	double rho=sqrt(2.*uconv)/hbarc*radius*
	  sqrt(redmass()*energy);
	double lValue=double(l);
	
	double eF,eG;
	gsl_sf_result F,Fp,G,Gp;
	
	gsl_sf_coulomb_wave_FG_e(eta,rho,lValue,0,&F,&Fp,&G,&Gp,&eF,&eG);
	
	struct CoulWaves newResult={F.val*exp(eF),Fp.val*exp(eF),G.val*exp(eG),Gp.val*exp(eG)};
	setLast(l,radius,energy,newResult);
	result=newResult;
      }
      return result;
    };
  /*!
   * Returns the penetrability as a function of orbital
   * angular momentum, radius, and energy in the center of mass system.
   */
    double Penetrability(int l,double radius,double energy)  {
      struct CoulWaves coul=this->operator()(l,radius,energy);
      double rho=sqrt(2.*uconv)/hbarc*radius*sqrt(redmass()*energy);
      return rho/(pow(coul.F,2.0)+pow(coul.G,2.0));
    };
  /*!
   * Returns the positive energy shift function a function of orbital
   * angular momentum, radius, and energy in the center of mass system.
   */
    double PEShift(int l,double radius,double energy)  {
      struct CoulWaves coul=this->operator()(l,radius,energy);
      double rho=sqrt(2.*uconv)/hbarc*radius*sqrt(redmass()*energy);
      return rho/(pow(coul.F,2.0)+pow(coul.G,2.0))*
	(coul.F*coul.dF+coul.G*coul.dG);
    };
  /*!
   * Returns the energy derivative of the shift function a function of orbital
   * angular momentum, radius, and energy in the center of mass system.
   */
    double PEShift_dE(int l,double radius,double energy)  {
      return gsl_PEshift_function_dE(this,l,radius,energy);
    };
  private:
    int z1_;
    int z2_;
    int lLast_;
    double redmass_;
    double radiusLast_;
    double energyLast_;
    struct CoulWaves coulLast_;
};

#endif
