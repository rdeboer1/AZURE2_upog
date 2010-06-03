#include "NFIntegral.h"

struct gsl_nf_integral_params {
  WhitFunc *theWhitFunc;
  int lfValue;
  double bindingEnergy;
  double whitChRadSquaredValue;
};

double gsl_nf_integrand(double x, void * p) {
  struct gsl_nf_integral_params *params = (struct gsl_nf_integral_params*)p;
  WhitFunc *theWhitFunc=(params->theWhitFunc);
  int lfValue=(params->lfValue);
  double bindingEnergy=(params->bindingEnergy);
  double whitChRadSquaredValue=(params->whitChRadSquaredValue);
  
  double whit=theWhitFunc->operator()(lfValue,x,bindingEnergy);
  return pow(whit,2.0)/whitChRadSquaredValue;
}

double gsl_nf_integral(WhitFunc *whit,int lf,double bindingEnergy,double chanRad) {
  
  double whitAtChRadSquared=pow(whit->operator()(lf,chanRad,bindingEnergy),2.0);  
				
  struct gsl_nf_integral_params params = {whit,lf,bindingEnergy,whitAtChRadSquared};
  
  gsl_integration_workspace * w 
    = gsl_integration_workspace_alloc (1000);
  
  gsl_function F;
  F.function = &gsl_nf_integrand;
  F.params= &params;

  double intresult,interror;

  gsl_integration_qagiu(&F,chanRad,0.0,1e-4,1000,w,&intresult,&interror);
  
  gsl_integration_workspace_free (w);
  
  return intresult;
}
