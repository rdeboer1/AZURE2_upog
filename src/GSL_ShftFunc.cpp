#include "ShftFunc.h"
#include <math.h>
#include <gsl/gsl_deriv.h>

struct gsl_shift_function_whit_params {
  int l;
  double bindingenergy;
  WhitFunc *whit;
};

double gsl_shift_function_whit(double x, void * p) {
  struct gsl_shift_function_whit_params *params =
    (struct gsl_shift_function_whit_params *)p;
  int l = (params->l);
  double bindingenergy = (params->bindingenergy);
  WhitFunc *whit = (params->whit);
  
  return whit->operator()(l,x,bindingenergy);
}

double gsl_shift_function(int l, double energy, double seperationenergy, 
			  double radius, WhitFunc *whit) {
  double bindingenergy=fabs(energy-seperationenergy);

  double result;
  double error;

  struct gsl_shift_function_whit_params 
    params={l,bindingenergy,whit};
 
  gsl_function F;
  F.function = &gsl_shift_function_whit;
  F.params = &params;
 
  gsl_deriv_central (&F, radius, 1e-4, &result, &error);

  return radius*result/gsl_shift_function_whit(radius,&params);
}
