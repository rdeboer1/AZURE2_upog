#include "EPoint.h"
#include <math.h>
#include <gsl/gsl_integration.h>

double StoppingPower(double energy);
double Straggling(double energy, double layerEnergy, double pointEnergy);
double TargetThickness(double energy);

struct gsl_target_integration_params {
  gsl_target_integration_params(const Config &a, CNuc *b, EPoint *c, double d) : 
    configure(a), compound(b), point(c), layerEnergy(d) {};
  const Config &configure;
  CNuc *compound;
  EPoint *point;
  double layerEnergy;
};
  
double gsl_straggling_integration_integrand(double x, void *p) {
  struct gsl_target_integration_params *params=(struct gsl_target_integration_params*)p;
  CNuc *compound=params->compound;
  const Config &configure = params->configure;
  EPoint *point = params->point;
  double layerEnergy=params->layerEnergy;

  double crossSection;
  if(x<50.0) {
    EPoint *newPoint= new EPoint(point->GetCMAngle(),x,point->GetEntranceKey(),point->GetExitKey(),
			point->IsDifferential(),point->IsPhase(),point->GetJ(),point->GetL());
    newPoint->Initialize(compound,configure);
    newPoint->Calculate(compound,configure);
    crossSection=newPoint->GetFitCrossSection();
	delete newPoint;
  } else crossSection=0.0;
  
  return crossSection/StoppingPower(x)*Straggling(x,layerEnergy,point->GetCMEnergy());
}

double gsl_target_integration_integrand(double x, void *p) {

  struct gsl_target_integration_params *params=(struct gsl_target_integration_params*)p;
  params->layerEnergy=x;

  gsl_integration_workspace * w 
    = gsl_integration_workspace_alloc (1000);
  gsl_function F;
  F.function=&gsl_straggling_integration_integrand;
  F.params=params;
 
  double result,error;
  gsl_integration_qagiu(&F,0.001,0.0,1e-5,1000,w,&result,&error);

  gsl_integration_workspace_free (w);

  return result;
}

double gsl_target_integration(EPoint *point,CNuc *compound, const Config&  configure) {
  struct gsl_target_integration_params params(configure,compound,point,point->GetCMEnergy());
  gsl_integration_workspace * w 
    = gsl_integration_workspace_alloc (1000);
  gsl_function F;
  F.function=&gsl_target_integration_integrand;
  F.params= &params;
  
  double pointEnergy=point->GetCMEnergy();
  double result,error;
  gsl_integration_qags(&F,pointEnergy-TargetThickness(pointEnergy),pointEnergy,0.0,1e-4,1000,w,&result,&error);

  gsl_integration_workspace_free (w);

  return result;
}



double StoppingPower(double energy) {
  double stoppingPower=0.;

  //code here, set stoppingPower as function of energy

  return stoppingPower;
}

double Straggling(double energy, double layerEnergy, double pointEnergy) {
  double straggling=0.;

  //code here, set straggling as function of energy

  return straggling;
}

double TargetThickness(double energy) {
  double targetThickness=0.;
  
  //code here, set TargetThickness as function of energy

  return targetThickness;
}
