#include <math.h>
#include <gsl/gsl_deriv.h>

extern double gsl_shift_function(int,double,double,double,double,int,int);

struct gsl_shift_function_dE_shift_params {
  int l;
  double seperationenergy;
  double radius;
  double redmass;
  int z1;
  int z2;
};

double gsl_shift_function_dE_shift (double x, void *p) {
  struct gsl_shift_function_dE_shift_params *params =
    (struct gsl_shift_function_dE_shift_params *)p;

  int l= (params->l);
  double seperationenergy = (params->seperationenergy);
  double radius = (params->radius);
  double redmass = (params->redmass);
  int z1 = (params->z1);
  int z2 = (params->z2);

  return gsl_shift_function(l,x,seperationenergy,radius,redmass,
		     z1,z2);
}

double gsl_shift_function_dE (int l, double energy, double seperationenergy, 
			      double radius, double redmass, int z1, int z2) {

  double result;
  double error;
   
  struct gsl_shift_function_dE_shift_params params = {
    l,seperationenergy,radius,redmass,z1,z2};
  
  gsl_function F;
  F.function = &gsl_shift_function_dE_shift;
  F.params = &params;

  gsl_deriv_central (&F, energy, 1e-6, &result, &error); 

  return result;
}
