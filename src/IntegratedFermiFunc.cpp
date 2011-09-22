#include "IntegratedFermiFunc.h"
#include <math.h>
#include <gsl/gsl_sf_gamma.h>
#include <gsl/gsl_integration.h>

IntegratedFermiFunc::IntegratedFermiFunc(double V0) :
  V0_(V0) {
}

double IntegratedFermiFunc::operator()(double W0, 
				       double Z, 
				       double radius) {
  
  if(W0<=1) return 0.;

  double gamma0 = sqrt(1.-alpha_*alpha_*Z*Z);
  double GammaDenom = gsl_sf_gamma(2.*gamma0+1.);
  Params_ params = {gamma0,
		    Z,
		    radius,
		    W0,
		    GammaDenom*GammaDenom,
		    V0_};

  gsl_integration_workspace * w 
    = gsl_integration_workspace_alloc (1000);
  gsl_function f;
  f.function = &IntegratedFermiFunc::Integrand;
  f.params = &params;
  
  double result, error;
  gsl_integration_qags (&f, 1., params.W0, 0., 1.e-6, 1000, w, &result, &error);
  
  gsl_integration_workspace_free (w);

  return result;
}


double IntegratedFermiFunc::Integrand(double x, void* p) {
  Params_* params = (Params_*)p;

  double W = x-params->V0*alpha_*alpha_*pow(params->Z,4./3.);
  double eta = alpha_*params->Z*W/sqrt(W*W-1.);
  gsl_sf_result GammaNumValue;
  gsl_sf_result GammaNumArg;
  gsl_sf_lngamma_complex_e(params->gamma0,eta,&GammaNumValue,&GammaNumArg);
  double GammaNum2 = exp(2.*GammaNumValue.val);
  
  double result = 
    2.*(1.+params->gamma0)*
    exp(pi_*eta-2.*(1.-params->gamma0)*
	log(2.*electronMass_*params->radius*sqrt(W*W-1.)/hbarc_))*
    GammaNum2/params->GammaDenom2*sqrt(W*W-1.)*W*(params->W0-W)*(params->W0-W);

  if(params->V0!=0.) {
    result*=sqrt((W*W-1.)/(x*x-1.))*W/x;
  }

  return result;
}
