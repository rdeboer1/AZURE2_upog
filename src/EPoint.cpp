#include "ESegment.h"

double gsl_target_integration(EPoint *point,CNuc *compound, const Config&  configure);

/*!
 * This constructor is used if the data point is to be created from a line in a data file.
 * A pointer to the parent segment is passed to the constructor for the intialization of
 * the EPoint object.
 */

EPoint::EPoint(DataLine dataLine, ESegment *parent) {
  entrance_key_=parent->GetEntranceKey();
  exit_key_=parent->GetExitKey();
  cm_angle_=dataLine.angle;
  lab_angle_=dataLine.angle;
  cm_energy_=dataLine.energy;
  lab_energy_=dataLine.energy;
  cm_crosssection_=dataLine.cross;
  cm_dcrosssection_=dataLine.error;
  lab_crosssection_=dataLine.cross;
  lab_dcrosssection_=dataLine.error;
  geofactor_=0.;
  fitcrosssection_=0.;
  sfactorconv_=0.;
  is_differential_=parent->IsDifferential();
  is_phase_=parent->IsPhase();
  j_value_=parent->GetJ();
  l_value_=parent->GetL();
  is_mapped_=false;
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
  cm_crosssection_=0.;
  cm_dcrosssection_=0.1;
  lab_crosssection_=0.;
  lab_dcrosssection_=0.1;
  geofactor_=0.;
  fitcrosssection_=0.;
  sfactorconv_=0.;
  is_differential_=parent->IsDifferential();
  is_phase_=parent->IsPhase();
  j_value_=parent->GetJ();
  l_value_=parent->GetL();
  is_mapped_=false;
}
  
/*!
 * This constructor is used if the data point is to be created with no experimental data
 * and no parent segment.  Such a point is used if AZURE is to be called as an energy-
 * dependent function for reaction rate and target effect calculations with dynamic
 * integration. All options must be set manually.
 */

EPoint::EPoint(double angle, double energy, int entranceKey, 
	       int exitKey, bool isDifferential, bool isPhase, double jValue, int lValue) {
  entrance_key_=entranceKey;
  exit_key_=exitKey;
  lab_angle_=angle;
  cm_angle_=angle;
  lab_energy_=energy;
  cm_energy_=energy;
  cm_crosssection_=0.;
  cm_dcrosssection_=0.1;
  lab_crosssection_=0.;
  lab_dcrosssection_=0.1;
  geofactor_=0.;
  fitcrosssection_=0.;
  sfactorconv_=0.;
  is_differential_=isDifferential;
  is_phase_=isPhase;
  j_value_=jValue;
  l_value_=lValue;
  is_mapped_=false;
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
 * Returns true if the point is a mapped point, otherwise returns false.  Mapping in 
 * AZURE is performed so calculations are not redundantly performed for like energies.
 * Energy dependent quantities are calculated only once for a given energy, and then 
 * copied to mapped points.
 */

bool EPoint::IsMapped() const {
  return is_mapped_;
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
  this->CalcEDependentValues(compound);
  if(this->IsDifferential()) 
    this->CalcLegendreP(configure.maxLOrder);
  this->CalcCoulombAmplitude(compound);
  if(configure.isEC) this->CalculateECAmplitudes(compound);
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

void EPoint::ConvertLabAngle(PPair *entrancePair, PPair *exitPair) {
  double qValue=entrancePair->GetSepE()+entrancePair->GetExE()-exitPair->GetSepE()-exitPair->GetExE();
  double a13=(entrancePair->GetM(1)*exitPair->GetM(1))*this->GetLabEnergy()/(this->GetLabEnergy()+qValue)/
    (entrancePair->GetM(1)+entrancePair->GetM(2))/(exitPair->GetM(1)+exitPair->GetM(2));
  double a24=(entrancePair->GetM(2)*exitPair->GetM(2))*(1+entrancePair->GetM(1)/entrancePair->GetM(2)*qValue/(this->GetLabEnergy()+qValue))/
    (entrancePair->GetM(1)+entrancePair->GetM(2))/(exitPair->GetM(1)+exitPair->GetM(2));

  if(a13>a24) {
    double thetaMax=asin(sqrt(a24/a13))*180./pi;
    if(thetaMax<this->GetLabAngle()) std::cout << std::endl << "Lab Angle (" << this->GetLabAngle() 
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

void EPoint::ConvertCrossSection() {
  double conversionFactor=pow(sin(pi/180.*this->GetLabAngle())/sin(pi/180.*this->GetCMAngle()),2.0)*cos(pi/180.*(this->GetCMAngle()-this->GetLabAngle()));
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
 * Calculates Legendre polynomials up to a maximum order.  The polynomials are added, in order, to a vector.
 */

void EPoint::CalcLegendreP(int maxL) {
  double x=cos(this->GetCMAngle()*pi/180.0);
  if(maxL>=0) {
    this->AddLegendreP(1.0);
    double polyMinusTwo=1.0;
    if(maxL>=1) {
      this->AddLegendreP(x);
      double polyMinusOne=x;
      if(maxL>=2) {
	for(int lOrder=2;lOrder<=maxL;lOrder++) {
	  double poly=(2.0*lOrder-1.0)/lOrder*x*polyMinusOne-
	    (lOrder-1.0)/lOrder*polyMinusTwo;
	  this->AddLegendreP(poly);
	  polyMinusTwo=polyMinusOne;
	  polyMinusOne=poly;
	}
      }
    }
  }
}

/*!
 * This function calculates several energy dependent quantities simultaniously.
 * This includes the geometrical cross section, the s-factor conversion, the \f$ L_o \f$ matrix
 * elements, the square root of the penetrability, and the exponentials of the Coulomb phase shifts
 * and hard sphere phase shfits.
 */

void EPoint::CalcEDependentValues(CNuc *theCNuc) {
  PPair *entrancePair=theCNuc->GetPair(theCNuc->GetPairNumFromKey(this->GetEntranceKey()));
  double geofactor=pi*pow(hbarc,2.)/(2*entrancePair->GetRedMass()*uconv*this->GetCMEnergy());
  this->SetGeometricalFactor(geofactor);
  double sfactorconv=this->GetCMEnergy()*exp(2*pi*sqrt(uconv/2.)*fstruc*entrancePair->GetZ(1)*
					      entrancePair->GetZ(2)*sqrt(entrancePair->GetRedMass()
									 /this->GetCMEnergy()));
  this->SetSFactorConversion(sfactorconv);
  double inEnergy=this->GetCMEnergy()+entrancePair->GetSepE()+entrancePair->GetExE();
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
	    CoulFunc theCoulombFunction(thePair);
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
	  complex loElement(0.0,0.0);
	  this->AddLoElement(j,ch,loElement);
	  this->AddSqrtPenetrability(j,ch,pow(localEnergy/hbarc, (double) lValue+0.5));
	  this->AddExpCoulombPhase(j,ch,1.0);
	  this->AddExpHardSpherePhase(j,ch,1.0);
	}
      }
    }
  }
  for(int i=1;i<=this->NumLocalMappedPoints();i++) {
    EPoint *mappedPoint=this->GetLocalMappedPoint(i);
    mappedPoint->geofactor_=geofactor_;
    mappedPoint->sfactorconv_=sfactorconv_;
    mappedPoint->lo_elements_=lo_elements_;
    mappedPoint->penetrabilities_=penetrabilities_;
    mappedPoint->coulombphase_=coulombphase_;
    mappedPoint->hardspherephase_=hardspherephase_;
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

void EPoint::CalculateECAmplitudes(CNuc *theCNuc) {
  int aa=theCNuc->GetPairNumFromKey(this->GetEntranceKey());
  if(theCNuc->GetPair(aa)->IsECEntrance()) {
    PPair *entrancePair=theCNuc->GetPair(aa);
    for(int ec=1;ec<=entrancePair->NumECLevels();ec++) {
      int ir=theCNuc->GetPairNumFromKey(this->GetExitKey());
      if(entrancePair->GetECLevel(ec)->GetPairNum()==ir) {
	double inEnergy=this->GetCMEnergy()+entrancePair->GetSepE()+entrancePair->GetExE();
	for(int k=1;k<=entrancePair->GetDecay(ir)->NumKGroups();k++) {
	  KGroup *theKGroup=entrancePair->GetDecay(ir)->GetKGroup(k);
	  for(int ecm=1;ecm<=theKGroup->NumECMGroups();ecm++) {
	    ECMGroup *theECMGroup=theKGroup->GetECMGroup(ecm);
	    //entrance Phase Calculations;
	    CoulFunc theCoulombFunction(entrancePair);
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
	    
	    double levelEnergy=theCNuc->GetJGroup(entrancePair->GetECLevel(ec)->GetJGroupNum())->
	      GetLevel(entrancePair->GetECLevel(ec)->GetLevelNum())->GetE();
	    double sqrtGammaPene=pow((inEnergy-levelEnergy)/hbarc,theECMGroup->GetMult()+0.5);
	    
	    //Initialize variables
	    AChannel *theFinalChannel = theCNuc->GetJGroup(entrancePair->GetECLevel(ec)->GetJGroupNum())->
	      GetChannel(theECMGroup->GetFinalChannel());
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
	    int theLMult=theECMGroup->GetMult();
	    int theFinalLValue=theFinalChannel->GetL();
	    double theFinalSValue=theFinalChannel->GetS();
	    double theInitialJValue=theECMGroup->GetJ();
	    double theFinalJValue=theCNuc->GetJGroup(entrancePair->GetECLevel(ec)->GetJGroupNum())->GetJ();
	    
	    double effectiveCharge;
	    if(theECMGroup->GetRadType()=='E') {
	      double totalM=theFinalPair->GetM(1)+theFinalPair->GetM(2);
	      effectiveCharge=sqrt(fstruc*hbarc)*(theFinalPair->GetZ(1)*pow(theFinalPair->GetM(2)/totalM,theLMult)+
						  theFinalPair->GetZ(2)*pow(-theFinalPair->GetM(1)/totalM,theLMult));
	    } else {
	      effectiveCharge=theFinalPair->GetRedMass()*1.00727638*
		(theFinalPair->GetZ(1)/pow(theFinalPair->GetM(1),2.)+
		 theFinalPair->GetZ(2)/pow(theFinalPair->GetM(2),2.));
	    }
	    
	    //calculate the FW and GW integrals
	    ECIntegral theECIntegral(theFinalPair);
	    struct ECIntResult integrals;
	    if(theECMGroup->GetRadType()=='E') integrals=theECIntegral(theInitialLValue,theFinalLValue,
								       theLMult,inEnergy,levelEnergy);
	    else integrals=theECIntegral(theInitialLValue,theFinalLValue,
					 0,inEnergy,levelEnergy);
	    
	    //calculate the total radial integral
	    complex overlapIntegral(0.,0.);
	    double outEnergy=inEnergy-theFinalPair->GetSepE()-theFinalPair->GetExE();
	    if(theECMGroup->IsChannelCapture()) {
	      if(outEnergy>0.0) {
		CoulFunc theChannelCoulombFunction(theFinalPair);
		struct CoulWaves 
		  chanCoul=theChannelCoulombFunction(theInitialLValue,theFinalPair->GetChRad(),
						     inEnergy-theFinalPair->GetSepE()-theFinalPair->GetExE());		
		complex chanExpHSP(chanCoul.G/sqrt(pow(chanCoul.F,2.0)+pow(chanCoul.G,2.0)),
						-chanCoul.F/sqrt(pow(chanCoul.F,2.0)+pow(chanCoul.G,2.0)));    
		overlapIntegral=complex(0.0,-0.5)*
		  sqrt(theChannelCoulombFunction.Penetrability(theInitialLValue,
							       theFinalPair->GetChRad(),
							       outEnergy))*
		  pow(theFinalPair->GetRedMass()*uconv/2./fabs(outEnergy),0.25)/sqrt(hbarc)*
		  chanExpHSP*(integrals.GW+complex(0.0,1.0)*integrals.FW);
	      } else {
		WhitFunc whitFunc(theFinalPair);
		double whit=whitFunc(theInitialLValue,theFinalPair->GetChRad(),fabs(outEnergy));
		overlapIntegral=complex(0.0,-0.5)*integrals.GW/whit*
		  sqrt(theFinalPair->GetRedMass()*uconv*theFinalPair->GetChRad()*uconv)/hbarc;
	      }
	    } else overlapIntegral=(coul.G/sqrt(pow(coul.F,2.0)+pow(coul.G,2.0))*integrals.FW
				    -coul.F/sqrt(pow(coul.F,2.0)+pow(coul.G,2.0))*integrals.GW)*
		pow(theFinalPair->GetRedMass()*uconv/2./fabs(outEnergy),0.25)/sqrt(hbarc);
	    
	    //calculate the total external capture amplitude for the pathway
	    complex ecAmplitude(0.0,0.0);
	    if(theECMGroup->GetRadType()=='E') {
	      ecAmplitude=complex(0.0,-1.0)*
		effectiveCharge*sqrt((8.*(2.*theLMult+1.)*(theLMult+1.))/theLMult)/DoubleFactorial(2*theLMult+1)*
		pow(complex(0.,1.0),theInitialLValue+theLMult-theFinalLValue)*
		ClebGord(theInitialLValue,theLMult,theFinalLValue,0,0,0)*sqrt(2.*theInitialLValue+1.)*sqrt(2.*theFinalJValue+1.)*
		Racah(theLMult,theFinalLValue,theInitialJValue,theInitialSValue,theInitialLValue,theFinalJValue);
	    } else {
	      complex orbitalTerm=effectiveCharge*
		sqrt((2.*theInitialLValue+1.)*(theInitialLValue+1.)*theInitialLValue)*
		Racah(1.,theInitialLValue,theInitialJValue,theInitialSValue,theInitialLValue,theFinalJValue);
	      complex tau=pow(std::complex<double>(-1.,0.),theFinalPair->GetJ(1)+theFinalPair->GetJ(2))*
		(pow(complex(-1.,0.),theFinalSValue)*
		 sqrt(theFinalPair->GetJ(1)*(theFinalPair->GetJ(1)+1.)*(2.*theFinalPair->GetJ(1)+1.))*
		 Racah(theFinalSValue,theFinalPair->GetJ(1),theInitialSValue,theFinalPair->GetJ(1),theFinalPair->GetJ(2),1.)*
		 theFinalPair->GetG(1)+
		 pow(complex(-1.,0.),theInitialSValue)*
		 sqrt(theFinalPair->GetJ(2)*(theFinalPair->GetJ(2)+1.)*(2.*theFinalPair->GetJ(2)+1.))*
		 Racah(theFinalSValue,theFinalPair->GetJ(2),theInitialSValue,theFinalPair->GetJ(2),theFinalPair->GetJ(1),1.)*
		 theFinalPair->GetG(2));
	      complex spinTerm=-sqrt((2.*theInitialSValue+1.)*(2.*theFinalSValue+1.))*
		Racah(1,theInitialSValue,theFinalJValue,theInitialLValue,theFinalSValue,theInitialJValue)*tau;
	      ecAmplitude=complex(0.0,1.0)*
		sqrt(fstruc)*pow(hbarc,1.5)/(2*1.00727638*uconv)*sqrt(16/3)*sqrt(2*theFinalJValue+1.)*
		(orbitalTerm+spinTerm);
	    }
	    ecAmplitude=ecAmplitude*expCP*expHSP*sqrtGammaPene*overlapIntegral;
	    this->AddECAmplitude(k,ecm,ecAmplitude);
	  }
	}
      }
    }
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

void EPoint::Calculate(CNuc* theCNuc,const Config &configure) {
  GenMatrixFunc *theMatrixFunc;
  if(configure.isAMatrix) theMatrixFunc=new AMatrixFunc(theCNuc);
  else theMatrixFunc=new RMatrixFunc(theCNuc);

  theMatrixFunc->ClearMatrices();
  theMatrixFunc->FillMatrices(this);
  theMatrixFunc->InvertMatrices();
  theMatrixFunc->CalculateTMatrix(this);
  theMatrixFunc->CalculateCrossSection(this);
  for(int i=1;i<=this->NumLocalMappedPoints();i++) {
    EPoint *mappedPoint = this->GetLocalMappedPoint(i);
    theMatrixFunc->CalculateCrossSection(mappedPoint);
  }

  delete theMatrixFunc;
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
 * A function that can replace EPoint::Calculate if target effects are to be included.  If implemented,
 * the user MUST set up the appropriate functions in the GSL_TargetIntegration.cpp file. 
 */

void EPoint::CalculateTargetEffects(CNuc* theCNuc,const Config &configure) {
  double yield=gsl_target_integration(this,theCNuc,configure);
  this->SetFitCrossSection(yield);
}

/*! 
 * Clears vector containing pointers to points mapped to the current point.
 */

void EPoint::ClearLocalMappedPoints() {
  local_mapped_points_.clear();
}

/*!
 * Returns a pointer to a point mapped to the current point specified by a position in the mapped point vector.
 */

EPoint* EPoint::GetLocalMappedPoint(int mappedPointNum) const {
  return local_mapped_points_[mappedPointNum-1];
}
