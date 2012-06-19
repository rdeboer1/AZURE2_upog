#include "AMatrixFunc.h"
#include "AngCoeff.h"
#include "CNuc.h"
#include "Config.h"
#include "CoulFunc.h"
#include "DataLine.h"
#include "ECIntegral.h"
#include "EData.h"
#include "ESegment.h"
#include "RMatrixFunc.h"
#include "ShftFunc.h"
#include "TargetEffect.h"
#include "IntegratedFermiFunc.h"
#include <iostream>
#include <assert.h>

/*!
 * This constructor is used if the data point is to be created from a line in a data file.
 * A pointer to the parent segment is passed to the constructor for the intialization of
 * the EPoint object.
 */

EPoint::EPoint(DataLine dataLine, ESegment *parent) {
  entrance_key_=parent->GetEntranceKey();
  exit_key_=parent->GetExitKey();
  cm_angle_=dataLine.angle();
  lab_angle_=dataLine.angle();
  cm_energy_=dataLine.energy();
  lab_energy_=dataLine.energy();
  excitation_energy_=dataLine.energy();
  cm_crosssection_=dataLine.crossSection();
  cm_dcrosssection_=dataLine.error();
  lab_crosssection_=dataLine.crossSection();
  lab_dcrosssection_=dataLine.error();
  geofactor_=0.;
  fitcrosssection_=0.;
  sfactorconv_=0.;
  is_differential_=parent->IsDifferential();
  is_phase_=parent->IsPhase();
  is_ang_dist_=parent->IsAngularDist();
  max_ang_dist_order_=parent->GetMaxAngDistOrder();
  j_value_=parent->GetJ();
  l_value_=parent->GetL();
  is_mapped_=false;
  targetEffectNum_=0;
  parentData_=NULL;
  stoppingPower_=0.0;
}
 
/*!
 * This constructor is used if the data point is to be created with no experimental data.
 * A pointer to the parent segment is passed to the constructor for the intialization of
 * the EPoint object.
 */

EPoint::EPoint(double angle, double energy, ESegment* parent) {
  entrance_key_=parent->GetEntranceKey();
  exit_key_=parent->GetExitKey();
  lab_angle_=angle;
  cm_angle_=angle;
  lab_energy_=energy;
  cm_energy_=energy;
  excitation_energy_=energy;
  cm_crosssection_=0.;
  cm_dcrosssection_=0.1;
  lab_crosssection_=0.;
  lab_dcrosssection_=0.1;
  geofactor_=0.;
  fitcrosssection_=0.;
  sfactorconv_=0.;
  is_differential_=parent->IsDifferential();
  is_phase_=parent->IsPhase();
  is_ang_dist_=parent->IsAngularDist();
  max_ang_dist_order_=parent->GetMaxAngDistOrder();
  j_value_=parent->GetJ();
  l_value_=parent->GetL();
  is_mapped_=false;
  targetEffectNum_=0;
  parentData_=NULL;
  stoppingPower_=0.0;
}
  
/*!
 * This constructor is used if the data point is to be created with no experimental data
 * and no parent segment.  Such a point is used if AZURE is to be called as an energy-
 * dependent function for reaction rate and target effect calculations with dynamic
 * integration. All options must be set manually.
 */

EPoint::EPoint(double angle, double energy, int entranceKey, 
	       int exitKey, bool isDifferential, bool isPhase, bool isAngularDist, double jValue, int lValue, int maxAngDistOrder) {
  entrance_key_=entranceKey;
  exit_key_=exitKey;
  lab_angle_=angle;
  cm_angle_=angle;
  lab_energy_=energy;
  cm_energy_=energy;
  excitation_energy_=energy;
  cm_crosssection_=0.;
  cm_dcrosssection_=0.1;
  lab_crosssection_=0.;
  lab_dcrosssection_=0.1;
  geofactor_=0.;
  fitcrosssection_=0.;
  sfactorconv_=0.;
  is_differential_=isDifferential;
  is_phase_=isPhase;
  is_ang_dist_=isAngularDist;
  max_ang_dist_order_=maxAngDistOrder;
  j_value_=jValue;
  l_value_=lValue;
  is_mapped_=false;
  targetEffectNum_=0;
  parentData_=NULL;
  stoppingPower_=0.0;
}    

/*!
 * Returns true if the point is differential cross section, otherwise returns false.
 */

bool EPoint::IsDifferential() const {
  return is_differential_;
}

/*!
 * Returns true if the point is phase shift, otherwise returns false.
 */

bool EPoint::IsPhase() const {
  return is_phase_;
}

/*!
 * Returns true if the point is angular distribution, otherwise returns false.
 */

bool EPoint::IsAngularDist() const {
  return is_ang_dist_;
}

/*!
 * Returns true if the point is a mapped point, otherwise returns false.  Mapping in 
 * AZURE is performed so calculations are not redundantly performed for like energies.
 * Energy dependent quantities are calculated only once for a given energy, and then 
 * copied to mapped points.
 */

bool EPoint::IsMapped() const {
  return is_mapped_;
}

/*!
 * This function retruns true if the point has a corresponding TargetEffect object, otherwise it returns false.
 */

bool EPoint::IsTargetEffect() const {
  if(GetTargetEffectNum()!=0) return true;
  else return false;
}

/*!
 * Returns the entrance particle pair key of the data point.  The key need not be the same
 * as the position of the pair in the PPair vector.  Pair keys are used in the setup files
 * of AZURE.
 */

int EPoint::GetEntranceKey() const {
  return entrance_key_;
}

/*!
 * Returns the exit particle pair key of the data point. The key need not be the same
 * as the position of the pair in the PPair vector.  Pair keys are used in the setup files
 * of AZURE.
 */

int EPoint::GetExitKey() const {
  return exit_key_;
}

/*!
 * The maximum order of the Legendre polynomials stored in the point object.
 */

int EPoint::GetMaxLOrder() const {
  return legendreP_.size()-1;
}

/*! 
 * Returns the orbital angular momentum value for the point.  Only applies if the point is
 * phase shift.
 */

int EPoint::GetL() const {
  return l_value_;
}

/*! 
 * Returns the number of points mapped to the current point.  
 */

int EPoint::NumLocalMappedPoints() const {
  return local_mapped_points_.size();
}

/*!
 * Returns the total number of sub-points contained within the present objet.  The sub-points are used to calculate the target effect integrals.
 */

int EPoint::NumSubPoints() const {
  return integrationPoints_.size();
}

/*! 
 * Returns the position of the corresponding TargetEffect object in the parent EData object.
 */

int EPoint::GetTargetEffectNum() const {
  return targetEffectNum_;
}

/*!
 * Returns the maximum polynomial order of the point is angular distribution.
 */

int EPoint::GetMaxAngDistOrder() const {
  return max_ang_dist_order_;
}

/*!
 * Return the number of angular distribution coefficients in the vector.
 */

int EPoint::GetNumAngularDists() const {
  return angularDists_.size();
}

/*! 
 * Returns the angle of the data point in the laboratory frame.
 */

double EPoint::GetLabAngle() const {
  return lab_angle_;
}

/*! 
 * Returns the angle of the data point in the center of mass frame.
 */

double EPoint::GetCMAngle() const {
  return cm_angle_;
}

/*! 
 * Returns the energy of the point in the laboratory frame.
 */

double EPoint::GetLabEnergy() const {
  return lab_energy_;
}

/*! 
 * Returns the energy of the point in the center of mass frame.
 */

double EPoint::GetCMEnergy() const {
  return cm_energy_;
}

/*! 
 * Returns the energy of the point in compound excitation energy.
 */

double EPoint::GetExcitationEnergy() const {
  return excitation_energy_;
}


/*! 
 * Returns the Legendre polynomial specified by an order.  
 */

double EPoint::GetLegendreP(int lOrder) const {
  return legendreP_[lOrder];
}

/*!
 * Returns the experimental cross section in the laboratory frame.
 */

double EPoint::GetLabCrossSection() const {
  return lab_crosssection_;
}

/*!
 * Returms the experimental cross section in the center of mass frame.
 */

double EPoint::GetCMCrossSection() const {
  return cm_crosssection_;
}

/*!
 * Returns the experimental uncertainty in the laboratory frame.
 */

double EPoint::GetLabCrossSectionError() const {
  return lab_dcrosssection_;
}

/*!
 * Returns the experimental uncertainty in the center of mass frame.
 */

double EPoint::GetCMCrossSectionError() const {
  return cm_dcrosssection_;
}

/*!
 * Returns the geometrical cross section factor \f$ \frac{\pi}{k^2} \f$.
 */

double EPoint::GetGeometricalFactor() const {
  return geofactor_;
}

/*!
 * Returns the calculated AZURE cross section.
 */

double EPoint::GetFitCrossSection() const {
  return fitcrosssection_;
}

/*!
 * Returns the multiplicative conversion factor from cross section to s-factor.
 */

double EPoint::GetSFactorConversion() const {
  return sfactorconv_;
}

/*!
 * Returns the square root of the penetrability for a channel specified by 
 * positions in the JGroup and subsequent AChannel vectors.
 */

double EPoint::GetSqrtPenetrability(int jGroupNum, int channelNum) const {
  return penetrabilities_[jGroupNum-1][channelNum-1];
}

/*!
 * Returns the total spin value for the data point.  Only applies if the
 * point is phase shift.
 */

double EPoint::GetJ() const {
  return j_value_;
}

/*!
 * For target integration to fit yield curves, the stopping power (or, rather, stopping cross section) is calculated and stored for each sub-point.  This function returns the precalculated value.
 */

double EPoint::GetStoppingPower() const {
	return stoppingPower_;
}

/*!
 * Returns the energy loss of the beam in the target for the current EPoint object.
 */

double EPoint::GetTargetThickness() const {
  return targetThickness_;
}

/*!
 * Returns the angular distribution coefficient corresponding to the given order;
 */

double EPoint::GetAngularDist(int order) const {
  return angularDists_[order];
}

/*!
 * Returns the \f$ L_o \f$ diagonal matrix element for a channel specified
 * by positions in the JGroup and subsequent AChannel vectors.
 */

complex EPoint::GetLoElement(int jGroupNum, int channelNum) const {
  return lo_elements_[jGroupNum-1][channelNum-1];
}

/*!
 * Returns the factor \f$ \exp(\omega_c) \f$ where \f$ \omega_c \f$ is the Coulomb 
 * phase shift.  The  channel us specified
 * by positions in the JGroup and subsequent AChannel vectors.
 */

complex EPoint::GetExpCoulombPhase(int jGroupNum, int channelNum) const {
  return coulombphase_[jGroupNum-1][channelNum-1];
}

/*!
 * Returns the factor \f$ \exp(\delta_c) \f$ where \f$ \delta_c \f$ is the hard sphere 
 * phase shift.  The  channel us specified
 * by positions in the JGroup and subsequent AChannel vectors.
 */

complex EPoint::GetExpHardSpherePhase(int jGroupNum, int channelNum) const {
  return hardspherephase_[jGroupNum-1][channelNum-1];
}

/*!
 * Returns the Coulomb amplitude \f$ C_\alpha \f$ for the data point.
 */

complex EPoint::GetCoulombAmplitude() const {
  return coulombamplitude_;
}

/*!
 * Returns the external capture amplitude for a given external reaction pathway
 * specified by positions in the KGroup and subsequent ECMGroup vectors.
 */

complex EPoint::GetECAmplitude(int kGroupNum, int ecMGroupNum) const {
  return ec_amplitudes_[kGroupNum-1][ecMGroupNum-1];
}

/*!
 * If a point is mapped, returns an EnergyMap structure containing the point to which it is mapped.
 */

EnergyMap EPoint::GetMap() const {
  EnergyMap thisMap;
  if(this->IsMapped()) {
    thisMap.segment=energy_map_.segment;
    thisMap.point=energy_map_.point;
  }
  else {
    thisMap.segment=0;
    thisMap.point=0;
  }
  return thisMap;
}

/*!
 * Initializes a data point to be used in a calculation.  Initilization is done before the fitting process
 * to calculate all energy dependent quantities that do no rely on the R-Matrix fit parameters.
 */

void EPoint::Initialize(CNuc *compound,const Config &configure) {
  this->CalcEDependentValues(compound,configure);
  if(this->IsDifferential()) 
    this->CalcLegendreP(configure.maxLOrder,NULL);
  this->CalcCoulombAmplitude(compound);
  if(configure.paramMask & Config::USE_EXTERNAL_CAPTURE) this->CalculateECAmplitudes(compound,configure);
}

/*!
 * Calculates center of mass energy.  When a data point is initialized, the same energy is copied into
 * the attributes for center of mass and lab energy.  If this function is called, the center of mass energy
 * attribute is overwritten with the value calculated from the lab energy attribute.
 */

void EPoint::ConvertLabEnergy(PPair *pPair) {
  cm_energy_=this->GetLabEnergy()*
    (pPair->GetM(2))/
    (pPair->GetM(1)+pPair->GetM(2));
  excitation_energy_=cm_energy_+pPair->GetSepE();
}

/*!
 * Calculates the total decay energy from the light particle decay energy, assuming the parent nucleus was at
 * rest when it decayed.  When a data point is initialized, the same energy is copied into
 * the attributes for center of mass and lab energy.  If this function is called, the center of mass energy
 * attribute is overwritten with the value calculated from the lab energy attribute.
 */

void EPoint::ConvertDecayEnergy(PPair *pPair) {
  cm_energy_=this->GetLabEnergy()/
    (pPair->GetM(2))*
    (pPair->GetM(1)+pPair->GetM(2));
  excitation_energy_=cm_energy_+pPair->GetSepE();
}

/*!
 * Calculates center of mass angles.  When a data point is initialized, the same angle is copied into
 * the attributes for center of mass and lab angles.  If this function is called, the center of mass angle
 * attribute is overwritten with the value calculated from the lab angle attribute.  This version of the
 * overloaded function is for scattering channels.
 */

void EPoint::ConvertLabAngle(PPair *pPair) {
  cm_angle_=this->GetLabAngle()+180./pi*asin(pPair->GetM(1)/pPair->GetM(2)*sin(pi/180.*this->GetLabAngle()));
}

/*!
 * Calculates center of mass angles.  When a data point is initialized, the same angle is copied into
 * the attributes for center of mass and lab angles.  If this function is called, the center of mass angle
 * attribute is overwritten with the value calculated from the lab angle attribute.  This version of the
 * overloaded function is for non-elastic particle channels.
 */

void EPoint::ConvertLabAngle(PPair *entrancePair, PPair *exitPair, const Config& configure) {
  double qValue=entrancePair->GetSepE()+entrancePair->GetExE()-exitPair->GetSepE()-exitPair->GetExE();
  double a13=(entrancePair->GetM(1)*exitPair->GetM(1))*this->GetLabEnergy()/(this->GetLabEnergy()+qValue)/
    (entrancePair->GetM(1)+entrancePair->GetM(2))/(exitPair->GetM(1)+exitPair->GetM(2));
  double a24=(entrancePair->GetM(2)*exitPair->GetM(2))*(1+entrancePair->GetM(1)/entrancePair->GetM(2)*qValue/(this->GetLabEnergy()+qValue))/
    (entrancePair->GetM(1)+entrancePair->GetM(2))/(exitPair->GetM(1)+exitPair->GetM(2));

  if(a13>a24) {
    double thetaMax=asin(sqrt(a24/a13))*180./pi;
    if(thetaMax<this->GetLabAngle()) configure.outStream << std::endl << "Lab Angle (" << this->GetLabAngle() 
					       << " degrees) is not kinematically possible.  Maximum angle is " 
					       << thetaMax << " degrees." << std::endl;
    assert(thetaMax>=this->GetLabAngle());
  }

  double E3PerEt=a13*pow(cos(this->GetLabAngle()*pi/180.)+sqrt(a24/a13-pow(sin(this->GetLabAngle()*pi/180.),2.0)),2.0);
  double tempE3PerEt = a13*pow(cos((this->GetLabAngle()+0.001)*pi/180.)+sqrt(a24/a13-pow(sin((this->GetLabAngle()+0.001)*pi/180.),2.0)),2.0);
  double slope = sqrt(tempE3PerEt/a24)*sin((this->GetLabAngle()+0.001)*pi/180.)-sqrt(E3PerEt/a24)*sin(this->GetLabAngle()*pi/180.);
  bool switchDomain=false;
  if(slope<0.) switchDomain=true;
  
  cm_angle_=180./pi*asin(sqrt(E3PerEt/a24)*sin(this->GetLabAngle()*pi/180.));
  if(switchDomain) cm_angle_ = 180.- cm_angle_;
}

/*!
 * Calculates center of mass cross sections.  When a data point is initialized, the same cross section is copied into
 * the attributes for center of mass and lab cross section.  If this function is called, the center of mass cross section
 * attribute is overwritten with the value calculated from the lab cross section attribute.
 */

void EPoint::ConvertCrossSection(PPair *entrancePair, PPair *exitPair) {
  double conversionFactor;
  if(this->GetLabAngle()==0.0 || this->GetLabAngle()==180.0) {
    double m1=entrancePair->GetM(1);
    double m2=entrancePair->GetM(2);
    double m3=exitPair->GetM(1);
    double m4=exitPair->GetM(2);
    double e1=this->GetLabEnergy();
    double qValue=entrancePair->GetSepE()+entrancePair->GetExE()-exitPair->GetSepE()-exitPair->GetExE();
    double et=e1+qValue;
    double a=m1*m4*e1/(m1+m2)/(m3+m4)/et;
    double b=m1*m3*e1/(m1+m2)/(m3+m4)/et;
    double c=m2*m3/(m1+m2)/(m3+m4) * (1+m1*qValue/m2/et);
    double d=m2*m4/(m1+m2)/(m3+m4) * (1+m1*qValue/m2/et);
    double e3et=b+d+2*pow(a*c,0.5)*cos(pi/180.*this->GetCMAngle());
    conversionFactor=pow(a*c,0.5)*pow(d/b-pow(sin(this->GetLabAngle()*pi/180.),2.0),0.5)/e3et;
  }
  else {
    conversionFactor=pow(sin(pi/180.*this->GetLabAngle())/sin(pi/180.*this->GetCMAngle()),2.0)*cos(pi/180.*(this->GetCMAngle()-this->GetLabAngle()));
  }
  cm_crosssection_=this->GetLabCrossSection()*conversionFactor;
  cm_dcrosssection_=this->GetLabCrossSectionError()*conversionFactor;
}

/*!
 * Adds a Legendre polynomial to the vector.  Polynomials are added to the vector in the order as L=0,1,2,...
 */

void EPoint::AddLegendreP(double polynomial) {
  legendreP_.push_back(polynomial);
}

/*!
 * Sets the geometrical cross section factor \f$ \frac{\pi}{k^2} \f$.
 */

void EPoint::SetGeometricalFactor(double geoFactor) {
  geofactor_=geoFactor;
}

/*!
 * Sets the calculated AZURE cross section.
 */

void EPoint::SetFitCrossSection(double crossSection) {
  fitcrosssection_=crossSection;
}

/*!
 * Sets the multiplicative conversion from cross section to s-factor.
 */

void EPoint::SetSFactorConversion(double conversion) {
  sfactorconv_=conversion;
}

/*!
 * Sets the exit key to the given value;
 */

void EPoint::SetExitKey(int key) {
  exit_key_=key;
}

/*!
 * Calculates Legendre polynomials up to a maximum order.  The polynomials are added, in order, to a vector.
 */

void EPoint::CalcLegendreP(int maxL,TargetEffect* targetEffect) {
  double x=cos(this->GetCMAngle()*pi/180.0);
  if(maxL>=0) {
    if(targetEffect && targetEffect->NumQCoefficients()>0)
      this->AddLegendreP(targetEffect->GetQCoefficient(0));
    else this->AddLegendreP(1.0);
    double polyMinusTwo=1.0;
    if(maxL>=1) {
      if(targetEffect && targetEffect->NumQCoefficients()>1)
	this->AddLegendreP(x*targetEffect->GetQCoefficient(1));
      else this->AddLegendreP(x);
      double polyMinusOne=x;
      if(maxL>=2) {
	for(int lOrder=2;lOrder<=maxL;lOrder++) {
	  double poly=(2.0*lOrder-1.0)/lOrder*x*polyMinusOne-
	    (lOrder-1.0)/lOrder*polyMinusTwo;
	  if(targetEffect && targetEffect->NumQCoefficients()>lOrder)
	    this->AddLegendreP(poly*targetEffect->GetQCoefficient(lOrder));
	  else this->AddLegendreP(poly);
	  polyMinusTwo=polyMinusOne;
	  polyMinusOne=poly;
	}
      }
    }
  }
  for(int i=1;i<=this->NumSubPoints();i++) {
    this->GetSubPoint(i)->CalcLegendreP(maxL, targetEffect);
  }
}

/*!
 * This function calculates several energy dependent quantities simultaniously.
 * This includes the geometrical cross section, the s-factor conversion, the \f$ L_o \f$ matrix
 * elements, the square root of the penetrability, and the exponentials of the Coulomb phase shifts
 * and hard sphere phase shfits.
 */

void EPoint::CalcEDependentValues(CNuc *theCNuc, const Config& configure) {
  PPair *entrancePair=theCNuc->GetPair(theCNuc->GetPairNumFromKey(this->GetEntranceKey()));
  PPair *exitPair=theCNuc->GetPair(theCNuc->GetPairNumFromKey(this->GetExitKey()));

  double inEnergy;
  double geofactor;
  double sfactorconv;
  if(theCNuc->GetPair(theCNuc->GetPairNumFromKey(this->GetEntranceKey()))->GetPType()==20)  {
    inEnergy=this->GetCMEnergy()+exitPair->GetSepE()+exitPair->GetExE();
    geofactor=1.;
    sfactorconv=1.;
  } else {
    inEnergy=this->GetCMEnergy()+entrancePair->GetSepE()+entrancePair->GetExE();
    geofactor=pi*pow(hbarc,2.)/(2*entrancePair->GetRedMass()*uconv*this->GetCMEnergy());
    sfactorconv=this->GetCMEnergy()*exp(2*pi*sqrt(uconv/2.)*fstruc*entrancePair->GetZ(1)*
					entrancePair->GetZ(2)*sqrt(entrancePair->GetRedMass()
								   /this->GetCMEnergy()));
  }
  this->SetGeometricalFactor(geofactor);
  this->SetSFactorConversion(sfactorconv);

  for(int j=1;j<=theCNuc->NumJGroups();j++) {
    if(theCNuc->GetJGroup(j)->IsInRMatrix()) {
      JGroup *theJGroup=theCNuc->GetJGroup(j);
      for(int ch=1;ch<=theJGroup->NumChannels();ch++) {
	AChannel *theChannel=theJGroup->GetChannel(ch);
	PPair *thePair=theCNuc->GetPair(theChannel->GetPairNum());
	int lValue=theChannel->GetL();
	double localEnergy=inEnergy-thePair->GetSepE()-thePair->GetExE();
	if(thePair->GetPType()==0) {
	  if(localEnergy<0.0) {
	    ShftFunc theShiftFunction(thePair);
	    double localShift=theShiftFunction(lValue,inEnergy);
	    double boundary=theChannel->GetBoundaryCondition();
	    complex loElement(localShift-boundary,0.0);
	    this->AddLoElement(j,ch,loElement);
	    this->AddSqrtPenetrability(j,ch,0.0);
	    this->AddExpCoulombPhase(j,ch,1.0);
	    this->AddExpHardSpherePhase(j,ch,1.0);
	  } else {
	    CoulFunc theCoulombFunction(thePair,!!(configure.paramMask&Config::USE_GSL_COULOMB_FUNC));
	    double radius=thePair->GetChRad();
	    double localPene=theCoulombFunction.Penetrability(lValue,radius,localEnergy);
	    double localShift=theCoulombFunction.PEShift(lValue,radius,localEnergy);
	    double boundary=theChannel->GetBoundaryCondition();
	    complex loElement(localShift-boundary,localPene);
	    double redmass=thePair->GetRedMass();
	    double eta=sqrt(uconv/2.)*fstruc*thePair->GetZ(1)*thePair->GetZ(2)*
	      sqrt(redmass/localEnergy);
	    complex expCP(1.0,0.0);
	    for(int ll=1;ll<=theChannel->GetL();ll++) 
	      expCP*=complex((double)ll/sqrt(pow(eta,2.0)+pow((double)ll,2.0)),
			     eta/sqrt(pow(eta,2.0)+pow((double)ll,2.0)));
	    struct CoulWaves 
	      coul=theCoulombFunction(lValue,radius,localEnergy);
	    complex expHSP(coul.G/sqrt(pow(coul.F,2.0)+pow(coul.G,2.0)),
			   -coul.F/sqrt(pow(coul.F,2.0)+pow(coul.G,2.0)));
	    this->AddLoElement(j,ch,loElement);
	    this->AddSqrtPenetrability(j,ch,sqrt(localPene));
	    this->AddExpCoulombPhase(j,ch,expCP);
	    this->AddExpHardSpherePhase(j,ch,expHSP);
	  }
	} else if(thePair->GetPType()==10){
	  complex loElement = complex(0.0,0.0);
	  this->AddLoElement(j,ch,loElement);
	  double sqrtPene = (configure.paramMask & Config::USE_RMC_FORMALISM) ? 1. : pow(localEnergy/hbarc, (double) lValue+0.5);
	  this->AddSqrtPenetrability(j,ch,sqrtPene);
	  this->AddExpCoulombPhase(j,ch,1.0);
	  this->AddExpHardSpherePhase(j,ch,1.0);
	} else if(thePair->GetPType()==20){
	  complex loElement = complex(0.0,0.0);
	  this->AddLoElement(j,ch,loElement);
	  IntegratedFermiFunc fermiFunc(thePair->GetZ(1));
	  double endPointE = thePair->GetSepE()-inEnergy;
	  double sqrtPene = (1.+endPointE/0.510998903<=1.) ? 0. : sqrt(fermiFunc(1.+endPointE/0.510998903,exitPair->GetZ(1)+exitPair->GetZ(2),thePair->GetChRad()));
	  this->AddSqrtPenetrability(j,ch,sqrtPene);
	  this->AddExpCoulombPhase(j,ch,1.0);
	  this->AddExpHardSpherePhase(j,ch,1.0);
	}
      }
    }
  }
  for(int i=1;i<=this->NumSubPoints();i++) {
    this->GetSubPoint(i)->CalcEDependentValues(theCNuc,configure);
  }
  for(int i=1;i<=this->NumLocalMappedPoints();i++) {
    EPoint *mappedPoint=this->GetLocalMappedPoint(i);
    mappedPoint->geofactor_=geofactor_;
    mappedPoint->sfactorconv_=sfactorconv_;
    mappedPoint->lo_elements_=lo_elements_;
    mappedPoint->penetrabilities_=penetrabilities_;
    mappedPoint->coulombphase_=coulombphase_;
    mappedPoint->hardspherephase_=hardspherephase_;
    for(int ii=1;ii<=this->NumSubPoints();ii++) {
      EPoint *subMappedPoint=mappedPoint->GetSubPoint(ii);
      subMappedPoint->geofactor_=this->GetSubPoint(ii)->geofactor_;
      subMappedPoint->sfactorconv_=this->GetSubPoint(ii)->sfactorconv_;
      subMappedPoint->lo_elements_=this->GetSubPoint(ii)->lo_elements_;
      subMappedPoint->penetrabilities_=this->GetSubPoint(ii)->penetrabilities_;
      subMappedPoint->coulombphase_=this->GetSubPoint(ii)->coulombphase_;
      subMappedPoint->hardspherephase_=this->GetSubPoint(ii)->hardspherephase_;
    }
  }
}

/*!
 * Adds an \f$ L_o \f$ matrix element with reference to positions in the JGroup and subsequent
 * AChannel vectors.  
 */

void EPoint::AddLoElement(int jGroupNum, int channelNum, complex loElement) {
  vector_c d;
  while(jGroupNum>lo_elements_.size()) lo_elements_.push_back(d);
  lo_elements_[jGroupNum-1].push_back(loElement);
  assert(channelNum=lo_elements_[jGroupNum-1].size());
}

/*!
 * Adds a square root of penetrability  with reference to positions in the JGroup and subsequent
 * AChannel vectors.  
 */

void EPoint::AddSqrtPenetrability(int jGroupNum, int channelNum, double sqrtPene) {
  vector_r d;
  while(jGroupNum>penetrabilities_.size()) penetrabilities_.push_back(d);
  penetrabilities_[jGroupNum-1].push_back(sqrtPene);
  assert(channelNum=penetrabilities_[jGroupNum-1].size());
}

/*!
 * Adds an exponential of the Coulomb phase shift with reference to positions in the JGroup and subsequent
 * AChannel vectors.  
 */

void EPoint::AddExpCoulombPhase(int jGroupNum, int channelNum, complex expShift) {
  vector_c d;
  while(jGroupNum>coulombphase_.size()) coulombphase_.push_back(d);
  coulombphase_[jGroupNum-1].push_back(expShift);
  assert(channelNum=coulombphase_[jGroupNum-1].size());
}

/*!
 * Adds an exponential of the hard sphere phase shift with reference to positions in the JGroup and subsequent
 * AChannel vectors.  
 */

void EPoint::AddExpHardSpherePhase(int jGroupNum, int channelNum, complex expShift) {
  vector_c d;
  while(jGroupNum>hardspherephase_.size()) hardspherephase_.push_back(d);
  hardspherephase_[jGroupNum-1].push_back(expShift);
  assert(channelNum=hardspherephase_[jGroupNum-1].size());
}

/*!
 * Calculates the Coulomb amplitude \f$ C_\alpha \f$ for the data point.
 */

void EPoint::CalcCoulombAmplitude(CNuc *theCNuc) {
  if(this->GetEntranceKey()==this->GetExitKey()) {
    PPair *entrancePair=theCNuc->GetPair(theCNuc->GetPairNumFromKey(this->GetEntranceKey()));
    int z1=entrancePair->GetZ(1);
    int z2=entrancePair->GetZ(2);
    double redmass=entrancePair->GetRedMass();
    double energy=this->GetCMEnergy();
    double angle=this->GetCMAngle();
    double eta=sqrt(uconv/2.)*fstruc*z1*z2*
      sqrt(redmass/energy);
    double cal=(1.0/(2.0*sqrt(pi)))*eta*(1.0/pow(sin(angle*pi/360.0),2.));
    double cex=2.0*eta*log(sin(angle*pi/360.0));
    complex calpha(cal*cos(cex),-cal*sin(cex));
    this->SetCoulombAmplitude(calpha);
  } else this->SetCoulombAmplitude(complex(0.,0.));
  for(int i=1;i<=this->NumSubPoints();i++) {
    this->GetSubPoint(i)->CalcCoulombAmplitude(theCNuc);
  }
}

/*!
 * Sets the Coulomb amplitude \f$ C_\alpha \f$ for the data point.
 */

void EPoint::SetCoulombAmplitude(complex amplitude) {
  coulombamplitude_=amplitude;
}

/*!
 * Calculates the external capture amplitudes for the data point.
 * The amplitudes are calculated for all reaction pathways with corresponding
 * entrance and exit pairs.
 */

void EPoint::CalculateECAmplitudes(CNuc *theCNuc, const Config& configure) {
  int aa=theCNuc->GetPairNumFromKey(this->GetEntranceKey());
  if(theCNuc->GetPair(aa)->GetPType()==20) return;
  if(theCNuc->GetPair(aa)->IsEntrance()) {
    PPair *entrancePair=theCNuc->GetPair(aa);
    for(int j=1;j<=theCNuc->NumJGroups();j++) {
      for(int la=1;la<=theCNuc->GetJGroup(j)->NumLevels();la++) {
	if(theCNuc->GetJGroup(j)->GetLevel(la)->IsECLevel()) {
	  ALevel *ecLevel = theCNuc->GetJGroup(j)->GetLevel(la);
	  int ir=theCNuc->GetPairNumFromKey(this->GetExitKey());
	  if(ecLevel->GetECPairNum()==ir) {
	    double inEnergy=this->GetCMEnergy()+entrancePair->GetSepE()+entrancePair->GetExE();
	    for(int k=1;k<=entrancePair->GetDecay(ir)->NumKGroups();k++) {
	      KGroup *theKGroup=entrancePair->GetDecay(ir)->GetKGroup(k);
	      for(int ecm=1;ecm<=theKGroup->NumECMGroups();ecm++) {
		ECMGroup *theECMGroup=theKGroup->GetECMGroup(ecm);
		//entrance Phase Calculations;
		CoulFunc theCoulombFunction(entrancePair,!!(configure.paramMask&Config::USE_GSL_COULOMB_FUNC));
		struct CoulWaves 
		  coul=theCoulombFunction(theECMGroup->GetL(),entrancePair->GetChRad(),
					  this->GetCMEnergy());		
		double eta=sqrt(uconv/2.)*fstruc*entrancePair->GetZ(1)*entrancePair->GetZ(2)*
		  sqrt(entrancePair->GetRedMass()/this->GetCMEnergy());
		complex expCP(1.0,0.0);
		for(int ll=1;ll<=theECMGroup->GetL();ll++) 
		  expCP*=complex((double)ll/sqrt(pow((double)ll,2.0)+pow(eta,2.0)),
				 eta/sqrt(pow((double)ll,2.0)+pow(eta,2.0)));
		complex expHSP(coul.G/sqrt(pow(coul.F,2.0)+pow(coul.G,2.0)),
			       -coul.F/sqrt(pow(coul.F,2.0)+pow(coul.G,2.0)));
		
		double levelEnergy=ecLevel->GetE();
		double sqrtGammaPene=pow((inEnergy-levelEnergy)/hbarc,theECMGroup->GetMult()+0.5);
		
		//Initialize variables
		AChannel *theFinalChannel = theCNuc->GetJGroup(j)->GetChannel(theECMGroup->GetFinalChannel());
		PPair *theFinalPair=theCNuc->GetPair(theFinalChannel->GetPairNum());		
		int theInitialLValue;
		double theInitialSValue;
		if(theECMGroup->IsChannelCapture()) {
		  MGroup *theChanCapMGroup=entrancePair->GetDecay(theECMGroup->GetChanCapDecay())->
		    GetKGroup(theECMGroup->GetChanCapKGroup())->GetMGroup(theECMGroup->GetChanCapMGroup());
		  theInitialLValue=theCNuc->GetJGroup(theChanCapMGroup->GetJNum())->
		    GetChannel(theChanCapMGroup->GetChpNum())->GetL();
		  theInitialSValue=theCNuc->GetJGroup(theChanCapMGroup->GetJNum())->
		    GetChannel(theChanCapMGroup->GetChpNum())->GetS();
		} else {
		  theInitialLValue=theECMGroup->GetL();
		  theInitialSValue=theKGroup->GetS();
		}	    
		
		ECIntegral theECIntegral(theFinalPair,configure);
		complex integrals = theECIntegral(theInitialLValue, theFinalChannel->GetL(), 
						  theInitialSValue, theFinalChannel->GetS(),
						  theECMGroup->GetJ(), theCNuc->GetJGroup(j)->GetJ(),
						  theECMGroup->GetMult(), theECMGroup->GetRadType(),
						  inEnergy, levelEnergy,
						  theECMGroup->IsChannelCapture());
		
		//calculate the total radial integral
		complex ecAmplitude=expCP*expHSP*sqrtGammaPene*integrals;
		this->AddECAmplitude(k,ecm,ecAmplitude);
	      }
	    }
	  }
	}
      }
    }
  }
  for(int i=1;i<=this->NumSubPoints();i++) {
    this->GetSubPoint(i)->CalculateECAmplitudes(theCNuc,configure);
  }
}

/*!
 * Adds an external capture amplitude with reference to a specified reaction pathway.
 */

void EPoint::AddECAmplitude(int kGroupNum, int ecMGroupNum, complex ecAmplitude) {
  vector_c d;
  while(kGroupNum>ec_amplitudes_.size()) ec_amplitudes_.push_back(d);
  ec_amplitudes_[kGroupNum-1].push_back(ecAmplitude);
  assert(ecMGroupNum=ec_amplitudes_[kGroupNum-1].size());
}

/*!
 * Calculates the cross section for a data point based on the fit parameters in the compound nucleus.
 */

void EPoint::Calculate(CNuc* theCNuc,const Config &configure, EPoint *parent, int subPointNum) {

  if(!this->IsTargetEffect()||
     (!this->GetParentData()->GetTargetEffect(this->GetTargetEffectNum())->IsConvolution()&&
      !this->GetParentData()->GetTargetEffect(this->GetTargetEffectNum())->IsTargetIntegration())) {
    GenMatrixFunc *theMatrixFunc;
    if(configure.paramMask & Config::USE_AMATRIX) theMatrixFunc=new AMatrixFunc(theCNuc,configure);
    else theMatrixFunc=new RMatrixFunc(theCNuc,configure);
    theMatrixFunc->ClearMatrices();
    theMatrixFunc->FillMatrices(this);
    theMatrixFunc->InvertMatrices();
    theMatrixFunc->CalculateTMatrix(this);
    theMatrixFunc->CalculateCrossSection(this);
    if(subPointNum&&parent) {
      for(int i=1;i<=parent->NumLocalMappedPoints();i++) {
	EPoint *mappedSubPoint = parent->GetLocalMappedPoint(i)->
	  GetSubPoint(subPointNum);
	theMatrixFunc->CalculateCrossSection(mappedSubPoint);
      }
    } else {
      for(int i=1;i<=this->NumLocalMappedPoints();i++) {
	EPoint *mappedPoint = this->GetLocalMappedPoint(i);
	theMatrixFunc->CalculateCrossSection(mappedPoint);
      }
    }
    delete theMatrixFunc;
  } else {
    for(int i = 1; i<=this->NumSubPoints();i++) {
      EPoint *subPoint=this->GetSubPoint(i);
      if(this->NumLocalMappedPoints()>0)
	subPoint->Calculate(theCNuc,configure,this,i);
      else subPoint->Calculate(theCNuc,configure);
    }
    this->IntegrateTargetEffect();
    for(int i=1;i<=this->NumLocalMappedPoints();i++)
      this->GetLocalMappedPoint(i)->IntegrateTargetEffect();
  }
}

/*!
 * If a point is mapped, sets the internal attribute indicating which point it is mapped to.
 */

void EPoint::SetMap(int segmentNum, int pointNum) {
  is_mapped_=true;
  energy_map_.segment=segmentNum;
  energy_map_.point=pointNum;
}

/*!
 * If a point is mapped to the current point, a pointer to the mapped point is added to a vector.
 */

void EPoint::AddLocalMappedPoint(EPoint *point) {
  local_mapped_points_.push_back(point);
}

/*! 
 * Clears vector containing pointers to points mapped to the current point.
 */

void EPoint::ClearLocalMappedPoints() {
  local_mapped_points_.clear();
}

/*!
 * Sets the position of the corresponding TargetEffect object in the parent EData object.
 */

void EPoint::SetTargetEffectNum(int targetEffectNum) {
  targetEffectNum_=targetEffectNum;
}

/*!
 * Adds a sub-point to the current point object for target effect integration.
 */

void EPoint::AddSubPoint(EPoint subPoint) {
  integrationPoints_.push_back(subPoint);
}

/*!
 * This function is called to integrate the vector of sub-points to determine 
 * the yield considering a given target effect. The function uses Simpson's 
 * rule to perform the integration.
 */

void EPoint::IntegrateTargetEffect() {
  double yield=0.0;
  TargetEffect *targetEffect=this->GetParentData()->GetTargetEffect(this->GetTargetEffectNum());
  double energyStep=this->GetSubPoint(1)->GetCMEnergy()-this->GetSubPoint(2)->GetCMEnergy();
  if(targetEffect->IsConvolution()&&targetEffect->IsTargetIntegration()) {
    int outerLowerLimit=round((this->GetSubPoint(1)->GetCMEnergy()-this->GetCMEnergy())/energyStep)+1;
    int outerUpperLimit=outerLowerLimit-1+round(this->GetTargetThickness()/energyStep);
    double outerIntFirst=0.0;
    double outerIntEvenSum=0.0;
    double outerIntOddSum=0.0;
    double outerIntegral=0.0;
    int outerCounter=0;
    for(int i=outerLowerLimit;i<=outerUpperLimit;i++) {
      int innerLowerLimit;
      if(i-round(targetEffect->convolutionRange*targetEffect->GetSigma()/energyStep)>1) 
	innerLowerLimit=i-round(targetEffect->convolutionRange*targetEffect->GetSigma()/energyStep);
      else innerLowerLimit=1;
      int innerUpperLimit;
      if(i+round(targetEffect->convolutionRange*targetEffect->GetSigma()/energyStep)-1<targetEffect->NumSubPoints())
	innerUpperLimit=i+round(targetEffect->convolutionRange*targetEffect->GetSigma()/energyStep)-1;
      else innerUpperLimit=targetEffect->NumSubPoints();
      double innerIntFirst=0.0;
      double innerIntEvenSum=0.0;
      double innerIntOddSum=0.0;
      double innerIntegral=0.0;
      double centroid=this->GetSubPoint(i)->GetCMEnergy();
      int innerCounter=0;
      for(int ii=innerLowerLimit;ii<=innerUpperLimit;ii++) {
	double thisEnergy=this->GetSubPoint(ii)->GetCMEnergy();
	double innerIntegrand=this->GetSubPoint(ii)->GetFitCrossSection()/
	  this->GetSubPoint(ii)->GetStoppingPower()/1e24*targetEffect->GetConvolutionFactor(thisEnergy,centroid);
	if(innerCounter==0) innerIntFirst=innerIntegrand;
	else if(innerCounter%2==0) {
	  innerIntEvenSum+=innerIntegrand;
	  if(innerCounter>=2) innerIntegral=energyStep/3.0*(innerIntFirst+4.0*innerIntOddSum+2.0*innerIntEvenSum-innerIntegrand);
	} else if(innerCounter%2!=0) {
	  innerIntOddSum+=innerIntegrand;
	  if(innerCounter>=2) innerIntegral=energyStep/3.0*(innerIntFirst+4.0*innerIntOddSum+2.0*innerIntEvenSum-3.0*innerIntegrand);
	}
	innerCounter++;
      }
      if(outerCounter==0) outerIntFirst=innerIntegral;
      else if(outerCounter%2==0) {
	outerIntEvenSum+=innerIntegral;
	if(outerCounter>=2) outerIntegral=energyStep/3.0*
		   (outerIntFirst+4.0*outerIntOddSum+2.0*outerIntEvenSum-innerIntegral);
      } else if(outerCounter%2!=0) {
	outerIntOddSum+=innerIntegral;
	if(outerCounter>=2) outerIntegral=energyStep/3.0*
		   (outerIntFirst+4.0*outerIntOddSum+2.0*outerIntEvenSum-3.0*innerIntegral);
      }
      outerCounter++;
    }
    yield=outerIntegral;
  } else if(targetEffect->IsConvolution()) {
    double intFirst=0.0;
    double intEvenSum=0.0;
    double intOddSum=0.0;
    double integral=0.0;
    double centroid=this->GetCMEnergy();
    for(int i=0;i<this->NumSubPoints();i++) {
      double thisEnergy=this->GetSubPoint(i+1)->GetCMEnergy();
      double integrand=this->GetSubPoint(i+1)->GetFitCrossSection()
	*targetEffect->GetConvolutionFactor(thisEnergy,centroid);
      if(i==0) intFirst=integrand;
      else if(i%2==0) {
	intEvenSum+=integrand;
	if(i>=2) integral=energyStep/3.0*(intFirst+4.0*intOddSum+2.0*intEvenSum-integrand);
      } else if(i%2!=0) {
	  intOddSum+=integrand;
	  if(i>=2) integral=energyStep/3.0*(intFirst+4.0*intOddSum+2.0*intEvenSum-3.0*integrand);
      }
    }
    yield=integral;
  } else if(targetEffect->IsTargetIntegration()) {
    double intFirst=0.0;
    double intEvenSum=0.0;
    double intOddSum=0.0;
    double integral=0.0;
    for(int i=0;i<this->NumSubPoints();i++) {
      double thisEnergy=this->GetSubPoint(i+1)->GetCMEnergy();
      double integrand=this->GetSubPoint(i+1)->GetFitCrossSection()/
	this->GetSubPoint(i+1)->GetStoppingPower()/1e24;
      if(i==0) intFirst=integrand;
      else if(i%2==0) {
	intEvenSum+=integrand;
	if(i>=2) integral=energyStep/3.0*(intFirst+4.0*intOddSum+2.0*intEvenSum-integrand);
      } else if(i%2!=0) {
	  intOddSum+=integrand;
	  if(i>=2) integral=energyStep/3.0*(intFirst+4.0*intOddSum+2.0*intEvenSum-3.0*integrand);
      }
    }
    yield=integral;    
  }
  this->SetFitCrossSection(yield);
}

/*!
 * This function sets an internal pointer to the parent EData object.
 */

void EPoint::SetParentData(EData* parentData) {
  parentData_=parentData;
}

/*!
 * This function sets the stopping cross section (effective) for the current
 * EPoint object.  Used only for sub-point involoved in target effect integration.
 */

void EPoint::SetStoppingPower(double stoppingPower) {
 stoppingPower_=stoppingPower;
}

/*!
 * This functions sets the energy loss of the beam in the target for the current EPoint object.
 */

void EPoint::SetTargetThickness(double targetThickness) {
  targetThickness_=targetThickness;
}

/*!
 * Sets the angular distribution coefficients.
 */

void EPoint::SetAngularDists(vector_r dists) {
  angularDists_.clear();
  angularDists_=dists;
}

/*!
 * Returns a pointer to the parent EData object.
 */

EData *EPoint::GetParentData() const {
  return parentData_;
}

/*!
 * Returns a pointer to a point mapped to the current point specified by a position in the mapped point vector.
 */

EPoint* EPoint::GetLocalMappedPoint(int mappedPointNum) const {
  return local_mapped_points_[mappedPointNum-1];
}

/*!
 * Returns a pointer to the specified sub-point in the current EPoint object.
 */

EPoint* EPoint::GetSubPoint(int subPoint) {
  EPoint *tempPoint;
  if(subPoint<=integrationPoints_.size()) tempPoint=&integrationPoints_[subPoint-1];
  else tempPoint= NULL;
  return tempPoint;
}

/*!
 * Returns a reference to the vector of EPoint objects containing the subpoints used in target
 * effect integration.
 */

std::vector<EPoint>& EPoint::GetSubPoints() {
  return integrationPoints_;
}

/*!
 * Returns a reference to the vector of pointers to mapped EPoint objects.
 */

std::vector<EPoint*>& EPoint::GetMappedPoints() {
  return local_mapped_points_;
}
