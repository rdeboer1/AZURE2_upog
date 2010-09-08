#include "Constants.h"
#include "CoulFunc.h"
#include <math.h>
#include <gsl/gsl_deriv.h>

struct gsl_PEshift_function_dE_PEshift_params {
  CoulFunc *theCoulFunc;
  int lValue;
  double radValue;
};

double gsl_PEshift_function_dE_PEshift (double x, void *p) {
  struct gsl_PEshift_function_dE_PEshift_params *params =
    (struct gsl_PEshift_function_dE_PEshift_params *)p;
  CoulFunc *theCoulFunc=(params->theCoulFunc);
  int lValue=(params->lValue);
  double radValue=(params->radValue);

  return theCoulFunc->PEShift(lValue,radValue,x);

}

double gsl_PEshift_function_dE(CoulFunc *coul, int l, double radius, double energy) {

  double result;
  double error;
  
  struct gsl_PEshift_function_dE_PEshift_params params = {coul,l,radius};
  
  gsl_function F;
  F.function=&gsl_PEshift_function_dE_PEshift;
  F.params=&params;

  gsl_deriv_central (&F, energy, 1e-6, &result, &error); 
  
  return result;
}
