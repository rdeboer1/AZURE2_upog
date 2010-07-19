#include <math.h>
#include <gsl/gsl_deriv.h>
#include "ShftFunc.h"

struct gsl_shift_function_dE_shift_params {
  int l;
  const ShftFunc *shift;
};

double gsl_shift_function_dE_shift (double x, void *p) {
  struct gsl_shift_function_dE_shift_params *params =
    (struct gsl_shift_function_dE_shift_params *)p;

  int l= (params->l);
  const ShftFunc *shift = (params->shift);

  return shift->operator()(l,x);
}

double gsl_shift_function_dE (int l, double energy, const ShftFunc* shift) {

  double result;
  double error;
   
  struct gsl_shift_function_dE_shift_params params = {
    l,shift};
  
  gsl_function F;
  F.function = &gsl_shift_function_dE_shift;
  F.params = &params;

  gsl_deriv_central (&F, energy, 1e-6, &result, &error); 

  return result;
}
