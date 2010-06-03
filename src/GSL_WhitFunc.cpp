#include <math.h>
#include <gsl/gsl_integration.h>
#include <gsl/gsl_sf_gamma.h>
#include "Constants.h"

struct gsl_whit_integrand_params {
  double a;
  double b;
  double z;
};

double gsl_whit_integrand(double x, void * p) {
  struct gsl_whit_integrand_params *params = (struct gsl_whit_integrand_params *)p;
  double a = (params->a);
  double b = (params->b);
  double z = (params->z);
  
  return exp(-z*x)*pow(x,a-1.0)*pow(1.0+x,b-a-1.0);
}

double gsl_whit_function(int l, double radius, double energy, double redmass, int z1, int z2) {

  const double k=-sqrt(uconv/2.)*fstruc*z1*z2*sqrt(redmass/energy);
  const double m=l+0.5;
  const double z=2.0*sqrt(2.0*uconv)/hbarc*radius*sqrt(redmass*energy);

  const double a=m-k+0.5;
  const double b=1.0+2.0*m;

  double intresult;
  double interror;

  struct gsl_whit_integrand_params params={a,b,z};

  gsl_integration_workspace * w 
    = gsl_integration_workspace_alloc (1000);

  gsl_function F;
  F.function = &gsl_whit_integrand;
  F.params = &params;

  gsl_integration_qagiu(&F,0.0,0.0,1e-4,1000,w,&intresult,&interror);
 
  gsl_integration_workspace_free (w);
 
  return exp(-z/2.0)*pow(z,m+0.50)/gsl_sf_gamma(a)*intresult;
}
