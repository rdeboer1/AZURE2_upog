#include <math.h>
#include <gsl/gsl_deriv.h>

extern double gsl_whit_function(int,double,double,double,int,int);

struct gsl_shift_function_whit_params {
  int l;
  double bindingenergy;
  double redmass;
  int z1;
  int z2;
};

double gsl_shift_function_whit(double x, void * p) {
  struct gsl_shift_function_whit_params *params =
    (struct gsl_shift_function_whit_params *)p;
  int l = (params->l);
  double bindingenergy = (params->bindingenergy);
  double redmass = (params->redmass);
  int z1 = (params->z1);
  int z2 = (params->z2);
 
  return gsl_whit_function(l,x,bindingenergy,redmass,z1,z2);
}

double gsl_shift_function(int l, double energy, double seperationenergy, 
			  double radius, double redmass, int z1, int z2) {
  double bindingenergy=fabs(energy-seperationenergy);

  double result;
  double error;

  struct gsl_shift_function_whit_params 
    params={l,bindingenergy,redmass,z1,z2};
 
  gsl_function F;
  F.function = &gsl_shift_function_whit;
  F.params = &params;
 
  gsl_deriv_central (&F, radius, 1e-4, &result, &error);

  return radius*result/gsl_shift_function_whit(radius,&params);
}
