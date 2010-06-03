#include <math.h>
#include <gsl/gsl_integration.h>
#include "ECIntegral.h"

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
