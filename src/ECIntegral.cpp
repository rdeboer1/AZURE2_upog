#include "ECIntegral.h"
#include "AngCoeff.h"
#include <math.h>
#include <gsl/gsl_integration.h>
#include <assert.h>

extern double DoubleFactorial(int);

struct gsl_ec_integral_params {
  CoulFunc *theCoulFunc;
  WhitFunc *theWhitFunc;
  int liValue;
  int lfValue;
  int multLValue;
  double pairEnergy;
  double bindingEnergy;
};

double gsl_ec_fw_integrand(double x, void * p) {
  struct gsl_ec_integral_params *params = (struct gsl_ec_integral_params *)p;
  CoulFunc *theCoulFunc=(params->theCoulFunc);
  WhitFunc *theWhitFunc=(params->theWhitFunc);
  int liValue = (params->liValue);
  int lfValue = (params->lfValue);
  int multLValue = (params->multLValue);
  double pairEnergy = (params->pairEnergy);
  double bindingEnergy = (params->bindingEnergy);
  
  struct CoulWaves coul = theCoulFunc->operator()(liValue,x,pairEnergy);
  double whit = theWhitFunc->operator()(lfValue,x,bindingEnergy);
  return coul.F*whit*pow(x,multLValue);
}

double gsl_ec_gw_integrand(double x, void * p) {
  struct gsl_ec_integral_params *params = (struct gsl_ec_integral_params *)p;
  CoulFunc *theCoulFunc=(params->theCoulFunc);
  WhitFunc *theWhitFunc=(params->theWhitFunc);
  int liValue = (params->liValue);
  int lfValue = (params->lfValue);
  int multLValue = (params->multLValue);
  double pairEnergy = (params->pairEnergy);
  double bindingEnergy = (params->bindingEnergy);
  
  struct CoulWaves coul = theCoulFunc->operator()(liValue,x,pairEnergy);
  double whit = theWhitFunc->operator()(lfValue,x,bindingEnergy);
  return coul.G*whit*pow(x,multLValue);
}
  
double gsl_ec_ww_integrand(double x, void * p) {
  struct gsl_ec_integral_params *params = (struct gsl_ec_integral_params *)p;
  WhitFunc *theWhitFunc=(params->theWhitFunc);
  int liValue = (params->liValue);
  int lfValue = (params->lfValue);
  int multLValue = (params->multLValue);
  double pairEnergy = (params->pairEnergy);
  double bindingEnergy = (params->bindingEnergy);

  double whitIn = theWhitFunc->operator()(liValue,x,fabs(pairEnergy));
  double whitOut= theWhitFunc->operator()(lfValue,x,fabs(bindingEnergy));
  return whitIn*whitOut*pow(x,multLValue);
}

struct ECIntResult gsl_ec_integral(CoulFunc *coul,WhitFunc *whit,int li,int lf, int multL,
		double energy,double bindingenergy, double chanrad) {

  struct gsl_ec_integral_params params = {coul,whit,li,lf,
					  multL,energy,bindingenergy};
  gsl_integration_workspace * w 
    = gsl_integration_workspace_alloc (1000);

  struct ECIntResult result;
  if(energy<0.0) {
    gsl_function WW;
    WW.function = &gsl_ec_ww_integrand;
    WW.params= &params;

    double wwintresult,wwinterror;

    gsl_integration_qagiu(&WW,chanrad,0.0,1e-4,1000,w,&wwintresult,&wwinterror);
    result.GW=wwintresult;
    result.FW=0.0;
  } else {
    gsl_function FW;
    FW.function = &gsl_ec_fw_integrand;
    FW.params= &params;

    gsl_function GW;
    GW.function = &gsl_ec_gw_integrand;
    GW.params= &params;


    double fwintresult,fwinterror;
    double gwintresult,gwinterror;

    gsl_integration_qagiu(&FW,chanrad,0.0,1e-4,1000,w,&fwintresult,&fwinterror);
    gsl_integration_qagiu(&GW,chanrad,0.0,1e-4,1000,w,&gwintresult,&gwinterror);
    result.FW=fwintresult;
    result.GW=gwintresult;
  }

  gsl_integration_workspace_free (w);
  
  return result;
}

  /*!
   * Overloaded operator to make the class instance callable as a function.  The intial and final orbital
   * angular momentum, the gamma multipolarity, and the incoming energy and final state energy in the 
   * compound system are the dependent variables. The function returns an ECIntResult structure.
   */  

complex ECIntegral::operator()(int theInitialLValue, int theFinalLValue, 
			       double theInitialSValue, double theFinalSValue,
			       double theInitialJValue, double theFinalJValue,
			       int theLMult, char radType,
			       double inEnergy, double levelEnergy,
			       bool isChannelCapture) const {


  double sepEnergy = pair()->GetSepE()+pair()->GetExE();
  double outEnergy = inEnergy - sepEnergy;
  double chanRad = pair()->GetChRad();
  double redMass = pair()->GetRedMass();

  struct ECIntResult integrals;
  if(radType=='E') 
    integrals=gsl_ec_integral(coulfunction(),whitfunction(),theInitialLValue,theFinalLValue,theLMult,
		    outEnergy,fabs(levelEnergy-sepEnergy),chanRad);
  else 
    integrals=gsl_ec_integral(coulfunction(),whitfunction(),theInitialLValue,theFinalLValue,0,
			      outEnergy,fabs(levelEnergy-sepEnergy),chanRad);

  complex overlapIntegral(0.,0.);
  if(outEnergy>0.0) {
    struct CoulWaves 
      coul=coulfunction()->operator()(theInitialLValue,chanRad,outEnergy);
    if(isChannelCapture) {	
      complex chanExpHSP(coul.G/sqrt(pow(coul.F,2.0)+pow(coul.G,2.0)),
			 -coul.F/sqrt(pow(coul.F,2.0)+pow(coul.G,2.0)));    
      overlapIntegral=complex(0.0,-0.5)*
	sqrt(coulfunction()->Penetrability(theInitialLValue,chanRad,outEnergy))*
	pow(redMass*uconv/2./fabs(outEnergy),0.25)/sqrt(hbarc)*
	chanExpHSP*(integrals.GW+complex(0.0,1.0)*integrals.FW);
    } else overlapIntegral=(coul.G/sqrt(pow(coul.F,2.0)+pow(coul.G,2.0))*integrals.FW
			    -coul.F/sqrt(pow(coul.F,2.0)+pow(coul.G,2.0))*integrals.GW)*
	     pow(redMass*uconv/2./fabs(outEnergy),0.25)/sqrt(hbarc);
  } else {
    assert(isChannelCapture);
    double whit=whitfunction()->operator()(theInitialLValue,chanRad,fabs(outEnergy));
    overlapIntegral=complex(0.0,-0.5)*integrals.GW/whit*
      sqrt(redMass*uconv*chanRad)/hbarc;
  }
 

  double effectiveCharge;
  if(radType=='E') {
    double totalM=pair()->GetM(1)+pair()->GetM(2);
    effectiveCharge=sqrt(fstruc*hbarc)*(pair()->GetZ(1)*pow(pair()->GetM(2)/totalM,theLMult)+
					pair()->GetZ(2)*pow(-pair()->GetM(1)/totalM,theLMult));
  } else {
    effectiveCharge=redMass*1.00727638*
      (pair()->GetZ(1)/pow(pair()->GetM(1),2.)+
       pair()->GetZ(2)/pow(pair()->GetM(2),2.));
  }

  complex ecAmplitude(0.0,0.0);
  if(radType=='E') {
    ecAmplitude=complex(0.0,-1.0)*
      effectiveCharge*sqrt((8.*(2.*theLMult+1.)*(theLMult+1.))/theLMult)/DoubleFactorial(2*theLMult+1)*
      pow(complex(0.,1.0),theInitialLValue+theLMult-theFinalLValue)*
      ClebGord(theInitialLValue,theLMult,theFinalLValue,0,0,0)*sqrt(2.*theInitialLValue+1.)*sqrt(2.*theFinalJValue+1.)*
      Racah(theLMult,theFinalLValue,theInitialJValue,theInitialSValue,theInitialLValue,theFinalJValue);
  } else {
    complex orbitalTerm=effectiveCharge*
      sqrt((2.*theInitialLValue+1.)*(theInitialLValue+1.)*theInitialLValue)*
      Racah(1.,theInitialLValue,theInitialJValue,theInitialSValue,theInitialLValue,theFinalJValue);
    complex tau=pow(std::complex<double>(-1.,0.),pair()->GetJ(1)+pair()->GetJ(2))*
      (pow(complex(-1.,0.),theFinalSValue)*
       sqrt(pair()->GetJ(1)*(pair()->GetJ(1)+1.)*(2.*pair()->GetJ(1)+1.))*
       Racah(theFinalSValue,pair()->GetJ(1),theInitialSValue,pair()->GetJ(1),pair()->GetJ(2),1.)*
       pair()->GetG(1)+
       pow(complex(-1.,0.),theInitialSValue)*
       sqrt(pair()->GetJ(2)*(pair()->GetJ(2)+1.)*(2.*pair()->GetJ(2)+1.))*
       Racah(theFinalSValue,pair()->GetJ(2),theInitialSValue,pair()->GetJ(2),pair()->GetJ(1),1.)*
       pair()->GetG(2));
    complex spinTerm=-sqrt((2.*theInitialSValue+1.)*(2.*theFinalSValue+1.))*
      Racah(1,theInitialSValue,theFinalJValue,theInitialLValue,theFinalSValue,theInitialJValue)*tau;
    ecAmplitude=complex(0.0,1.0)*
      sqrt(fstruc)*pow(hbarc,1.5)/(2*1.00727638*uconv)*sqrt(16/3)*sqrt(2*theFinalJValue+1.)*
      (orbitalTerm+spinTerm);
  }	    
  
  return ecAmplitude*overlapIntegral;
};
