#include "AZUREOutput.h"
#include "CNuc.h"
#include "Config.h"
#include "EData.h"
#include "ExtrapLine.h"
#include "SegLine.h"
#include "Minuit2/MnUserParameters.h"
#include <iostream>
#include <iomanip>
#include <omp.h>
#include <time.h>

/*!
 * The EData object has a private attribute containing the number of iterations needed to find the best fit parameters.
 * At creation, this attribute is set to 0.
 */

EData::EData() {
  iterations_=0;
  isFit_=true;
  isErrorAnalysis_=false;
}

/*!
 * Returns the number of segment objects in the ESegment vector.
 */

int EData::NumSegments() const {
  return segments_.size();
}

/*!
 * This function fills the data object with segments from the segment data files.
 * After a segment is created, the ESegment::Fill method is called for that segment.
 * Returns -1 if the input files could not be read, otherwise returns 0.
 */

int EData::Fill(const struct Config& configure, CNuc *theCNuc) {
  std::ifstream in(configure.configfile.c_str());
  if(!in) return -1;
  std::string line="";
  while(line!="<segmentsData>"&&!in.eof()) getline(in,line);
  if(line!="<segmentsData>") return -1;
  line="";
  int numTotalSegments=0;
  while(!in.eof()&&line!="</segmentsData>") {
    getline(in,line);
    bool empty=true;
    for(unsigned int i=0;i<line.size();++i) 
      if(line[i]!=' '&&line[i]!='\t') {
	empty=false;
	break;
      }
    if(empty==true) continue;
    if(!in.eof()&&line!="</segmentsData>") {
      std::istringstream stm;
      stm.str(line);
      SegLine segment(stm);
      if(stm.rdstate() & (std::stringstream::failbit | std::stringstream::badbit)) return -1;
      numTotalSegments++;
      if(segment.isActive()==1) {
	ESegment NewSegment(segment);
	if(theCNuc->IsPairKey(NewSegment.GetEntranceKey())) {
	  theCNuc->GetPair(theCNuc->GetPairNumFromKey(NewSegment.GetEntranceKey()))->SetEntrance();
	  if(theCNuc->IsPairKey(NewSegment.GetExitKey())) {
	    NewSegment.SetSegmentKey(numTotalSegments);
	    this->AddSegment(NewSegment);
	    if(this->GetSegment(this->NumSegments())->Fill(theCNuc,this)==-1) {
              std::cout << "Could Not Fill Segment " << this->NumSegments() 
			<< " from file." << std::endl;
	    } 
	  } else std::cout << "Pair key " << NewSegment.GetExitKey() 
			   << " not in compound nucleus." << std::endl;
	} else std::cout << "Pair key " << NewSegment.GetEntranceKey() 
			 << " not in compound nucleus." << std::endl;
      }
    }
  }

  if(line!="</segmentsData>") return -1;

  in.close();
  if(this->ReadTargetEffectsFile(configure.configfile,theCNuc)==-1) return -1;
  this->MapData();

  return 0; 
}

/*!
 * If the AZURE calculation is not data driven, this function is called in place
 * of the EData::Fill function to create points at specified energies and angles.
 * Returns -1 if the input files could not be read, otherwise returns 0.
 */

int EData::MakePoints(const struct Config& configure, CNuc *theCNuc) {
  std::ifstream in(configure.configfile.c_str());
  if(!in) return -1;
  std::string line = "";
  while(line!="<segmentsTest>"&&!in.eof()) getline(in,line);
  if(line!="<segmentsTest>") return -1;
  line="";
  int numTotalSegments=0;
  while(!in.eof()&&line!="</segmentsTest>") {
    getline(in,line);
    bool empty=true;
    for(unsigned int i=0;i<line.size();++i) 
      if(line[i]!=' '&&line[i]!='\t') {
	empty=false;
	break;
      }
    if(empty==true) continue;
    if(!in.eof()&&line!="</segmentsTest>") {
      std::istringstream stm;
      stm.str(line);
      ExtrapLine segment(stm);
      if(stm.rdstate() & (std::stringstream::failbit | std::stringstream::badbit)) return -1;
      if(segment.isActive()==1) {
	numTotalSegments++;
	ESegment NewSegment(segment);
	if(theCNuc->IsPairKey(NewSegment.GetEntranceKey())) {
	  if(theCNuc->IsPairKey(NewSegment.GetExitKey())) {
	    NewSegment.SetSegmentKey(numTotalSegments);
	    this->AddSegment(NewSegment);
	    ESegment *theSegment=this->GetSegment(this->NumSegments());
	    theCNuc->GetPair(theCNuc->GetPairNumFromKey(theSegment->GetEntranceKey()))->SetEntrance();
	    PPair *entrancePair=theCNuc->GetPair(theCNuc->GetPairNumFromKey(theSegment->GetEntranceKey()));
	    PPair *exitPair=theCNuc->GetPair(theCNuc->GetPairNumFromKey(theSegment->GetExitKey()));
	    double aStep=theSegment->GetAStep();
	    double eStep=theSegment->GetEStep();
	    for(double angle=theSegment->GetMinAngle();
		angle<=theSegment->GetMaxAngle();angle+=aStep) {
	      for(double energy=theSegment->GetMinEnergy();
		  energy<=theSegment->GetMaxEnergy();energy+=eStep) {
		EPoint NewPoint(angle,energy,theSegment);
		theSegment->AddPoint(NewPoint);
		EPoint *thePoint=theSegment->GetPoint(theSegment->NumPoints());
		thePoint->SetParentData(this);
		thePoint->ConvertLabEnergy(entrancePair);
		if(exitPair->GetPType()==0&&theSegment->IsDifferential()&&!theSegment->IsPhase()) {
		  if(theSegment->GetEntranceKey()==theSegment->GetExitKey()) {
		    thePoint->ConvertLabAngle(entrancePair);
		  } else {
		    thePoint->ConvertLabAngle(entrancePair,exitPair);
		  }
		  thePoint->ConvertCrossSection();
		}
		if(eStep==0.0) break;
	      }
	      if(aStep==0.0) break;
	    }
	  } else std::cout << "Pair key " << NewSegment.GetExitKey() 
			   << " not in compound nucleus." << std::endl;
	} else std::cout << "Pair key " << NewSegment.GetEntranceKey() 
			 << " not in compound nucleus." << std::endl;
      }
    }
  }  

  if(line!="</segmentsTest>") return -1;
  
  in.close();
  if(this->ReadTargetEffectsFile(configure.configfile,theCNuc)==-1) return -1;
  this->MapData();
  return 0; 
}

/*!
 * Returns the number of fit iterations needed to minimize the parameters to the data.
 */

int EData::Iterations() const {
  return iterations_;
}

/*!
 * Returns the number of TargetEffect objects contained in the present object.
 */

int EData::NumTargetEffects() const {
  return targetEffects_.size();
}

/*!
 * Returns the offset of the normalization paramters in the Minuit parameter vector.
 */
int EData::GetNormParamOffset() const {
  return normParamOffset_;
}

/*!
 * Reads the target effects input file and creates the TargetEffect objects 
 * to be applied to the data.
 */

int EData::ReadTargetEffectsFile(std::string infile, CNuc *compound) {
  std::ifstream in(infile.c_str());
  if(!in) return -1;
  std::string line="";
  while(line!="<targetInt>"&&!in.eof()) getline(in,line);
  if(line!="<targetInt>") return -1;
  line="";
  while(line!="</targetInt>"&&!in.eof()) {
    getline(in,line);
    bool empty=true;
    for(unsigned int i=0;i<line.size();++i) 
      if(line[i]!=' '&&line[i]!='\t') {
	empty=false;
	break;
      }
    if(empty==true) continue;
    if(line!="</targetInt>"&&!in.eof()){
      std::istringstream stm;
      stm.str(line);
      TargetEffect targetEffect(stm);
      if(stm.rdstate() & (std::stringstream::failbit | std::stringstream::badbit)) return -1;
      if(targetEffect.IsActive()) {
	this->AddTargetEffect(targetEffect);
	TargetEffect *thisTargetEffect=this->GetTargetEffect(this->NumTargetEffects());
	std::vector<int> segmentsList = thisTargetEffect->GetSegmentsList();
	for(int i = 1;i<=segmentsList.size();i++) { 
	  if(this->IsSegmentKey(segmentsList[i-1]))
	    this->GetSegmentFromKey(segmentsList[i-1])->SetTargetEffectNum(this->NumTargetEffects());
	}
      }
    }
  }
  if(line!="</targetInt>") return -1;
  for(ESegmentIterator segment=GetSegments().begin();segment<GetSegments().end();segment++) {
    PPair *entrancePair = compound->GetPair(compound->GetPairNumFromKey(segment->GetEntranceKey()));
    double cmConversion = entrancePair->GetM(2)/(entrancePair->GetM(1)+entrancePair->GetM(2));
    if(segment->IsTargetEffect()) {
      TargetEffect *targetEffect = this->GetTargetEffect(segment->GetTargetEffectNum());
      for(EPointIterator point=segment->GetPoints().begin();point<segment->GetPoints().end();point++) {
	point->SetTargetEffectNum(segment->GetTargetEffectNum());
	double forwardDepth=0.0;
	double backwardDepth=0.0;
	if(targetEffect->IsTargetIntegration()) {
	  double totalM=entrancePair->GetM(1)+entrancePair->GetM(2);
	  double targetThickness = cmConversion*targetEffect->TargetThickness(point->GetLabEnergy());
	  point->SetTargetThickness(targetThickness);
	  if(targetEffect->IsConvolution()) {
	    backwardDepth=targetThickness+targetEffect->convolutionRange*targetEffect->GetSigma();
	    forwardDepth=targetEffect->convolutionRange*targetEffect->GetSigma();
	  } else {
	    backwardDepth=targetThickness;
	    forwardDepth=0.0;
	  }
	} else if(targetEffect->IsConvolution()) {
	  backwardDepth=targetEffect->convolutionRange*targetEffect->GetSigma();
	  forwardDepth=targetEffect->convolutionRange*targetEffect->GetSigma();
	}
	for(int i=0;i<targetEffect->NumSubPoints();i++) {
	  double subEnergy=point->GetCMEnergy()+forwardDepth
	    -(forwardDepth+backwardDepth)/(targetEffect->NumSubPoints())*i;
	  EPoint subPoint(point->GetCMAngle(),subEnergy,&*segment);
	  if(targetEffect->IsTargetIntegration()) {
	  	double stoppingPower=cmConversion*targetEffect->
	  	GetStoppingPowerEq()->Evaluate(subEnergy/cmConversion);
  	    subPoint.SetStoppingPower(stoppingPower);
	  }
	  point->AddSubPoint(subPoint);
	}
      }
    }
  }
  return 0;
}

/*!
 * Returns true if the data is to be fit, otherwise returns false.  Used in the AZURECalc function class
 * to determine if a clone of the CNuc and EData objects should be made for thread safety.
 */

bool EData::IsFit() const {
  return isFit_;
}

/*!
 * Returns true if the call to function is for error analysis via Minos, otherwise returns false.  Used in the AZURECalc function
 * class to suppress transformation and file output during error analysis.
 */

bool EData::IsErrorAnalysis() const {
  return isErrorAnalysis_;
}

/*!
 * Sets the boolean indicating if the data is to be fit by AZURECalc function class. Used in AZUREMain function
 * class before calls to Minuit and AZURECalc.
 */

/*!
 * Returns true if the specified segment key exists corresponds to a segment in the ESegment vector, 
 * otherwise returns false.
 */

bool EData::IsSegmentKey(int segmentKey) {
  bool isKey=false;
  for (ESegmentIterator segment=GetSegments().begin();segment<GetSegments().end();segment++) {
    if(segment->GetSegmentKey()==segmentKey) {
      isKey=true;
      break;
    }
  }
  return isKey;
}

/*!
 * Sets an internal variable specifying if the data is to be fit by Minuit.  Needed to determine cloning behavior in AZURECalc for 
 * thread safety.
 */

void EData::SetFit(bool fit) {
  isFit_=fit;
}

/*!
 * Sets the boolean indicating if the call to the function is for error analysis via Minos.
 */

void EData::SetErrorAnalysis(bool errorAnalysis) {
  isErrorAnalysis_=errorAnalysis;
}

/*!
 * This function updates the number of fit iterations per iteration during the fitting process.
 */

void EData::Iterate(){
  iterations_++;
}

/*!
 * This function sets the number of iterations to zero.
 */

void EData::ResetIterations(){
  iterations_=0;
}

/*!
 * This function is identical in role to the EPoint::Initialize function, except that it initializes
 * and entire EData object instead of a single EPoint object.
 */

void EData::Initialize(CNuc *compound,const struct Config &configure) {
  //Calculate channel lo-matrix and channel penetrability for each channel at each local energy
  std::cout << "Calculating Lo-Matrix, Phases, and Penetrabilities..." << std::endl;
  this->CalcEDependentValues(compound,configure);
  if((configure.fileCheckMask|configure.screenCheckMask) & Config::CHECK_ENERGY_DEP) 
    this->PrintEDependentValues(configure,compound);

  //Calculate legendre polynomials for each data point
  std::cout << "Calculating Legendre Polynomials..." << std::endl;
  this->CalcLegendreP(configure.maxLOrder);
  if((configure.fileCheckMask|configure.screenCheckMask) & Config::CHECK_LEGENDRE) 
    this->PrintLegendreP(configure);

  //Calculate Coulomb Amplitudes
  std::cout << "Calculating Coulomb Amplitudes..." << std::endl;
  this->CalcCoulombAmplitude(compound);
  if((configure.fileCheckMask|configure.screenCheckMask) & Config::CHECK_COUL_AMPLITUDES) {
    this->PrintCoulombAmplitude(configure,compound);
  }

  //Calculate new ec amplitudes
  if(configure.paramMask & Config::USE_EXTERNAL_CAPTURE) {
    std::cout << "Calculating External Capture Amplitudes..." << std::endl;
    this->CalculateECAmplitudes(compound,configure);
  }
}

/*!
 * Adds a segment to the ESegment vector.
 */

void EData::AddSegment(ESegment segment) {
  segments_.push_back(segment);
}

/*!
 * Prints the data point after the object is filled or points are created.
 */

void EData::PrintData(const struct Config &configure) {
  std::streambuf *sbuffer;
  std::filebuf fbuffer;
  if(configure.fileCheckMask & Config::CHECK_DATA) {
    std::string outfile=configure.checkdir+"data.chk";
    fbuffer.open(outfile.c_str(),std::ios::out);
    sbuffer = &fbuffer;
  } else if(configure.screenCheckMask & Config::CHECK_DATA) sbuffer = std::cout.rdbuf();
  std::ostream out(sbuffer);
  if(((configure.fileCheckMask & Config::CHECK_DATA)&&fbuffer.is_open())||
     (configure.screenCheckMask & Config::CHECK_DATA)) {
    out << std::endl
	<< "************************************" << std::endl
	<< "*            Segments              *" << std::endl
	<< "************************************" << std::endl;
    out << std::setw(11) << "Segment #"
	<< std::setw(17) << "Segment Key #"
	<< std::setw(17) << "Entrance Key #"
	<< std::setw(13) << "Exit Key #"
	<< std::setw(12) << "Min Energy"
	<< std::setw(12) << "Max Energy"
	<< std::setw(11) << "Min Angle"
	<< std::setw(11) << "Max Angle"
	<< std::setw(25) << "Data File"
	<< std::endl; 
    for(ESegmentIterator segment=GetSegments().begin();segment<GetSegments().end();segment++) {
      out << std::setw(11) << segment-GetSegments().begin()+1
	  << std::setw(17) << segment->GetSegmentKey()
	  << std::setw(17) << segment->GetEntranceKey()
	  << std::setw(13) << segment->GetExitKey()
	  << std::setw(12) << segment->GetMinEnergy()
	  << std::setw(12) << segment->GetMaxEnergy()
	  << std::setw(11) << segment->GetMinAngle()
	  << std::setw(11) << segment->GetMaxAngle()
	  << std::setw(25) << segment->GetDataFile()
	  << std::endl;
    }  
    out << std::endl
	<< "************************************" << std::endl
	<< "*               Data               *" << std::endl
	<< "************************************" << std::endl;
    out << std::setw(11) << "Segment #"
	<< std::setw(14) << "Data Point #"
	<< std::setw(15) << "Lab Energy"
	<< std::setw(15) << "CM Energy"
	<< std::setw(15) << "Angle"
	<< std::setw(20) << "Cross Section"
	<< std::setw(22) << "Cross Section Error"
        << std::setw(12) << "Map Point"
	<< std::setw(18) << "# of Subpoints"
	<< std::setw(18) << "Low Sub Energy"
	<< std::setw(18) << "High Sub Energy"
	<< std::endl;
    for(EDataIterator data=begin();data!=end();data++) {
      out << std::setw(11) << data.segment()-GetSegments().begin()+1
	  << std::setw(14) << data.point()-(data.segment()->GetPoints()).begin()+1
	  << std::setw(15) << data.point()->GetLabEnergy() 
	  << std::setw(15) << data.point()->GetCMEnergy() 
	  << std::setw(15) << data.point()->GetCMAngle()
	  << std::setw(20) << data.point()->GetCMCrossSection()
	  << std::setw(22) << data.point()->GetCMCrossSectionError();
      if(data.point()->IsMapped()){
	EnergyMap map=data.point()->GetMap();
	char tempMap[25];
	sprintf(tempMap,"(%d,%d)",map.segment,map.point);
	out << std::setw(12) <<  tempMap << std::endl;
      } else
	out << std::setw(12) << "Not Mapped"
	    << std::setw(18) << data.point()->NumSubPoints();
      if(data.point()->IsTargetEffect()) {
	out << std::setw(18) << data.point()->GetSubPoint(data.point()->NumSubPoints())->GetCMEnergy() 
	    << std::setw(18) << data.point()->GetSubPoint(1)->GetCMEnergy();
      }
      out << std::endl;
      if(data.point()==data.segment()->GetPoints().end()-1) out << std::endl;
    }
  } else std::cout << "Could not write data check file." << std::endl;
  out.flush();
  if(fbuffer.is_open()) fbuffer.close();
}

/*!
 * Calls EPoint::CalcLegendreP for each point in the entire EData object.
 */

void EData::CalcLegendreP(int maxL) {

  std::map<int,std::vector<double> > qCoeffs;
  //std::vector<double> segQCoeffs;
  //Insert segment QCoeffs IN ORDER here with: segQCoeffs.push_back(qValue);
  // segQCoeffs.push_back(1.0);  //first L=0
  // segQCoeffs.push_back(1.0);  //then  L=1, etc.
  //Then insert segQCoeff into qCoeffs with: qCoeffs[segmentKey]=segQCoeffs;
  // qCoeffs[1]=segQCoeffs;  
  //clear segQCoeffs with segQCoeffs.clear() and repeat for another segment
  
  for(ESegmentIterator segment=GetSegments().begin();segment<GetSegments().end();segment++) { 
    std::map<int,std::vector<double> >::iterator qCoeffsItr = qCoeffs.find(segment->GetSegmentKey());
    std::vector<double>* qCoeffsPtr = (qCoeffsItr!=qCoeffs.end()) ? &qCoeffsItr->second : NULL;
#pragma omp parallel for 
    for(int i=1;i<=segment->NumPoints();i++) {
      EPoint* point=segment->GetPoint(i);
      point->CalcLegendreP(maxL,qCoeffsPtr);    
    }
  }
}

/*!
 * Prints the Legendre polynomials for each point in the EData object.
 */ 

void EData::PrintLegendreP(const struct Config &configure) {
  std::streambuf *sbuffer;
  std::filebuf fbuffer;
  if(configure.fileCheckMask & Config::CHECK_LEGENDRE) {
    std::string outfile=configure.checkdir+"legendre.chk";
    fbuffer.open(outfile.c_str(),std::ios::out);
    sbuffer = &fbuffer;
  } else if(configure.screenCheckMask & Config::CHECK_LEGENDRE) sbuffer = std::cout.rdbuf();
  std::ostream out(sbuffer);
  if(((configure.fileCheckMask & Config::CHECK_LEGENDRE)&&fbuffer.is_open())||
     (configure.screenCheckMask & Config::CHECK_LEGENDRE)) {
    out << std::endl
    << "************************************" << std::endl
    << "*       Legendre Polynomials       *" << std::endl
    << "************************************" << std::endl;
    out << std::setw(10) << "Segment #"
    << std::setw(10) << "Point #"
    << std::setw(15) << "CM Energy"
    << std::setw(15) << "Angle"
    << std::setw(5)  << "L"
    << std::setw(15) << "Leg. Poly." << std::endl;
    for(EDataIterator data=begin();data!=end();data++) {
      for(int lOrder=0;lOrder<=data.point()->GetMaxLOrder();lOrder++) {
	out << std::setw(10) << data.segment()-GetSegments().begin()+1
	    << std::setw(10) << data.point()-(data.segment()->GetPoints()).begin()+1
	    << std::setw(15) << data.point()->GetCMEnergy()
	    << std::setw(15) << data.point()->GetCMAngle()
	    << std::setw(5)  << lOrder
	    << std::setw(15) << data.point()->GetLegendreP(lOrder) << std::endl;	
      }
    }
  } else std::cout << "Could not write legendre polynomials check file." << std::endl;
  out.flush();
  if(fbuffer.is_open()) fbuffer.close();    
}

/*!
 * Calls EPoint::CalcEDependentValues for each point in the entire EData object.
 */

void EData::CalcEDependentValues(CNuc *theCNuc,const struct Config& configure) {
  for(ESegmentIterator segment=GetSegments().begin(); segment<GetSegments().end(); segment++) {
#pragma omp parallel for
  	for(int i=1;i<=segment->NumPoints();i++) {
      EPoint *point = segment->GetPoint(i);
      if(!(point->IsMapped())) point->CalcEDependentValues(theCNuc,configure);
    }
  }
}

/*!
 * Prints the values calculated by EPoint::CalcEDependentValues for each point in the entire EData object.
 */

void EData::PrintEDependentValues(const struct Config &configure,CNuc *theCNuc) {
  std::streambuf *sbuffer;
  std::filebuf fbuffer;
  if(configure.fileCheckMask & Config::CHECK_ENERGY_DEP) {
    std::string outfile=configure.checkdir+"lomatrixandpene.chk";
    fbuffer.open(outfile.c_str(),std::ios::out);
    sbuffer = &fbuffer;
  } else if(configure.screenCheckMask & Config::CHECK_ENERGY_DEP) sbuffer = std::cout.rdbuf();
  std::ostream out(sbuffer);
  if(((configure.fileCheckMask & Config::CHECK_ENERGY_DEP)&&fbuffer.is_open())||
     (configure.screenCheckMask & Config::CHECK_ENERGY_DEP)) {
    out << std::endl
    << "************************************" << std::endl
    << "*  Lo Matrix and Penetrabilities   *" << std::endl
    << "************************************" << std::endl;
    out << std::setw(10) << "Seg #"
    << std::setw(10) << "Point #"
    << std::setw(5)  << "j" 
    << std::setw(5)  << "ch" 
    << std::setw(5)  << "l"
    << std::setw(15) << "E chan"
    << std::setw(15) << "pene" 
    << std::setw(25) << "Lo" << std::endl;
    for(EDataIterator data=begin();data!=end();data++) {
      double inEnergy=data.point()->GetCMEnergy()
        +theCNuc->GetPair(theCNuc->GetPairNumFromKey(data.segment()->GetEntranceKey()))->GetSepE();
      for(int j=1;j<=theCNuc->NumJGroups();j++) {
	if(theCNuc->GetJGroup(j)->IsInRMatrix()) {
	  JGroup *theJGroup=theCNuc->GetJGroup(j);
	  for(int ch=1;ch<=theJGroup->NumChannels();ch++) {
	    AChannel *theChannel=theJGroup->GetChannel(ch);
	    PPair *thePair=theCNuc->GetPair(theChannel->GetPairNum());
	    int lValue=theChannel->GetL();
	    double localEnergy=inEnergy-thePair->GetSepE()-thePair->GetExE();
	    out << std::setw(10) << data.segment()-GetSegments().begin()+1
		<< std::setw(10) << data.point()-(data.segment()->GetPoints()).begin()+1
		<< std::setw(5) << j 
		<< std::setw(5) << ch  
		<< std::setw(5) << lValue 
		<< std::setw(15) << localEnergy 
		<< std::setw(15) << data.point()->GetSqrtPenetrability(j,ch)
		<< std::setw(25) << data.point()->GetLoElement(j,ch) << std::endl;
	  }           
	}
      }
    }   
  } else std::cout << "Could not write lo-matrix and penetrabilities check file." << std::endl;
  out.flush();
  if(fbuffer.is_open()) fbuffer.close();    
}

/*!
 * Calls EPoint::CalcCoulombAmplitude for each point in the entire EData object.
 */

void EData::CalcCoulombAmplitude(CNuc *theCNuc) {
  for(ESegmentIterator segment=GetSegments().begin();segment<GetSegments().end();segment++) {
#pragma omp parallel for
    for(int i=1;i<=segment->NumPoints();i++) {
      EPoint* point = segment->GetPoint(i);
      point->CalcCoulombAmplitude(theCNuc);
    }
  }
}

/*!
 * Prints the values calculated by EPoint::CalcCoulombAmplitude for each point in the entire EData object.
 */

void EData::PrintCoulombAmplitude(const struct Config &configure,CNuc *theCNuc) {
  std::streambuf *sbuffer;
  std::filebuf fbuffer;
  if(configure.fileCheckMask & Config::CHECK_COUL_AMPLITUDES) {
    std::string outfile=configure.checkdir+"coulombamplitudes.chk";
    fbuffer.open(outfile.c_str(),std::ios::out);
    sbuffer = &fbuffer;
  } else if(configure.screenCheckMask & Config::CHECK_COUL_AMPLITUDES) sbuffer = std::cout.rdbuf();
  std::ostream out(sbuffer);
  if(((configure.fileCheckMask & Config::CHECK_COUL_AMPLITUDES)&&fbuffer.is_open())||
     (configure.screenCheckMask & Config::CHECK_COUL_AMPLITUDES)) {
    out << std::endl
	<< "************************************" << std::endl
	<< "*        Coulomb Amplitudes        *" << std::endl
	<< "************************************" << std::endl;
    out << std::setw(10) << "segment #"
	<< std::setw(10) << "point #"
	<< std::setw(10) << "aa"
	<< std::setw(15) << "cmenergy"
	<< std::setw(15) << "angle"
	<< std::setw(25) << "coulomb amplitude"
	<< std::endl;
    for(ESegmentIterator segment=GetSegments().begin();segment<GetSegments().end();segment++) {
      if(segment->GetEntranceKey()==segment->GetExitKey()) {
	for(EPointIterator point=segment->GetPoints().begin();point<segment->GetPoints().end();point++) {
	  out << std::setw(10) << segment-GetSegments().begin()+1
	      << std::setw(10) << point-segment->GetPoints().begin()+1
	      << std::setw(10) << theCNuc->GetPairNumFromKey(segment->GetEntranceKey())
	      << std::setw(15) << point->GetCMEnergy()
	      << std::setw(15) << point->GetCMAngle()
	      << std::setw(25) << point->GetCoulombAmplitude()
	      << std::endl;
	}
      }
    }
  } else std::cout << "Could not write coulomb amplitudes check file." << std::endl;
  out.flush();
  if(fbuffer.is_open()) fbuffer.close();
}

/*!
 * Writes the output files for the calculation.  The output files are all in center of mass frame, and contain
 * columns for energy, angle, calculated cross section, calculated s-factor, experimental cross section and error
 * and experimental s-factor and error.
 */

void EData::WriteOutputFiles(const struct Config &configure, bool isFit) {
  AZUREOutput output(configure.outputdir);
  std::ofstream chiOut;
  if(!isFit&&(configure.paramMask & Config::CALCULATE_WITH_DATA)) {
    std::string chiOutFile = configure.outputdir+"chiSquared.out";
    chiOut.open(chiOutFile.c_str());
  }
  if(configure.paramMask ^ Config::CALCULATE_WITH_DATA) output.SetExtrap();
  bool isVaryNorm=false;
  double totalChiSquared=0.;
  for(ESegmentIterator segment=GetSegments().begin();
      segment<GetSegments().end();segment++) {
    if(segment->IsVaryNorm()) isVaryNorm=true;
    int aa=segment->GetEntranceKey();
    int ir=segment->GetExitKey();
    std::ostream out(output(aa,ir));
    for(EPointIterator point=segment->GetPoints().begin();point<segment->GetPoints().end();point++) {
      out.precision(4);
      out << std::setw(13) << std::scientific << point->GetCMEnergy()
	  << std::setw(13) << std::scientific << point->GetCMAngle()
	  << std::setw(13) << std::scientific << point->GetFitCrossSection()
	  << std::setw(13) << std::scientific << point->GetFitCrossSection()*point->GetSFactorConversion();
      if(!output.IsExtrap()) {
      double dataNorm=segment->GetNorm();
      out << std::setw(13) << std::scientific << point->GetCMCrossSection()*dataNorm
	  << std::setw(13) << std::scientific << point->GetCMCrossSectionError()*dataNorm
	  << std::setw(13) << std::scientific << point->GetCMCrossSection()*dataNorm*point->GetSFactorConversion()
	  << std::setw(13) << std::scientific << point->GetCMCrossSectionError()*dataNorm*point->GetSFactorConversion()
	  << std::endl;
      } else out << std::endl;
    }
    if(!isFit&&(configure.paramMask & Config::CALCULATE_WITH_DATA)) {
      totalChiSquared+=segment->GetSegmentChiSquared();
      chiOut << "Segment #"
	     << segment->GetSegmentKey() 
	     << " Chi-Squared/N: "
	     << segment->GetSegmentChiSquared()/segment->NumPoints()
	     << std::endl;
    }
    out<<std::endl<<std::endl;out.flush();
  }
  if(!isFit&&(configure.paramMask & Config::CALCULATE_WITH_DATA)) {
    chiOut << "Total Chi-Squared: " 
	      << totalChiSquared << std::endl << std::endl;
    chiOut.flush();chiOut.close();
  }  
  if(isVaryNorm) {
    std::string outputfile=configure.outputdir+"normalizations.out";
    std::ofstream out(outputfile.c_str());
    if(out) {
      out.precision(4);
      out << std::fixed;
      for(ESegmentIterator segment=GetSegments().begin();segment<GetSegments().end();segment++) {
	if(segment->IsVaryNorm()) out << std::setw(20) << "Segment Key #" << segment->GetSegmentKey()
				      << std::setw(10) << segment->GetNorm() << std::endl;
      }
      out.flush();
      out.close();
    } else std::cout << "Could not write normalization file." << std::endl;
  }
}

/*!
 * If external capture amplitudes are to be calculated, EPoint::CalculateECAmplitudes is
 * called for each point with a corresponding external capture component in the EData object.
 * Otherwise, the amplitudes are read from the specified file.
 */

void EData::CalculateECAmplitudes(CNuc *theCNuc,const struct Config& configure) {
  std::ifstream in;
  std::ofstream out;
  std::string outputfile;
  if(configure.paramMask & Config::CALCULATE_WITH_DATA) outputfile=configure.outputdir+"intEC.dat";
  else outputfile=configure.outputdir+"intEC.extrap";
  if(configure.paramMask & Config::USE_PREVIOUS_INTEGRALS) in.open(configure.integralsfile.c_str());
  else {
    out.open(outputfile.c_str());
    if(!out) std::cout << "Could not write to EC Amplitude File." << std::endl;
  }
  for(ESegmentIterator segment=GetSegments().begin();segment<GetSegments().end();segment++) {
    int aa=theCNuc->GetPairNumFromKey(segment->GetEntranceKey());
    if(theCNuc->GetPair(aa)->IsEntrance()) {
      PPair *entrancePair=theCNuc->GetPair(aa);
      for(int j=1;j<=theCNuc->NumJGroups();j++) {
	for(int la=1;la<=theCNuc->GetJGroup(j)->NumLevels();la++) {
	  if(theCNuc->GetJGroup(j)->GetLevel(la)->IsECLevel()) {
	    ALevel *ecLevel = theCNuc->GetJGroup(j)->GetLevel(la);
	    int ir=theCNuc->GetPairNumFromKey(segment->GetExitKey());
	    if(ecLevel->GetECPairNum()==ir) {
	      if(configure.paramMask ^ Config::USE_PREVIOUS_INTEGRALS) {
		std::cout << "\tSegment #" << std::setw(3) << segment->GetSegmentKey() 
		          << std::setw(0) << " [                         ] 0%";std::cout.flush();
		int numPoints=segment->NumPoints();
		int pointIndex=0;
		time_t startTime = time(NULL);
#pragma omp parallel for
		for(int i=1;i<=numPoints;i++) {
		  EPoint *point = segment->GetPoint(i);
		  if(!(point->IsMapped())) point->CalculateECAmplitudes(theCNuc);
		  ++pointIndex;
		  if(difftime(time(NULL),startTime)>0.25) {
		  	  startTime=time(NULL);
		      std::string progress=" [";
		  	  double percent=0.;
		      for(int j = 1;j<=25;j++) {
			    if(pointIndex>=percent*numPoints&&percent<1.) {
			    	percent+=0.04;
			    	progress+='*';
			    } else progress+=' ';
		      } progress+="] ";
		      std::cout << "\r\tSegment #" << std::setw(3) << segment->GetSegmentKey() 
		      	        << std::setw(0) << progress << percent*100 << '%';std::cout.flush();
		  }
		}
	    std::cout << "\r\tSegment #" << std::setw(3) << segment->GetSegmentKey() 
	              << std::setw(0) << " [*************************] 100%" << std::endl;
	      }
	      for(EPointIterator point=segment->GetPoints().begin();
		  point<segment->GetPoints().end();point++) {
		if(!(point->IsMapped())) {
		  for(int k=1;k<=entrancePair->GetDecay(ir)->NumKGroups();k++) {
		    for(int ecm=1;ecm<=entrancePair->GetDecay(ir)->GetKGroup(k)->NumECMGroups();ecm++) {
		      if(configure.paramMask ^ Config::USE_PREVIOUS_INTEGRALS) {
			if(out.is_open()) out << point->GetECAmplitude(k,ecm) << std::endl;
			for(EPointIterator subPoint=point->GetSubPoints().begin();
			    subPoint<point->GetSubPoints().end();subPoint++)
			  if(out.is_open()) out << subPoint->GetECAmplitude(k,ecm) << std::endl;
		      } else {
			complex ecAmplitude(0.0,0.0);
			in >> ecAmplitude;
			point->AddECAmplitude(k,ecm,ecAmplitude);
			for(EPointIterator subPoint=point->GetSubPoints().begin();
			    subPoint<point->GetSubPoints().end();subPoint++) {
			  ecAmplitude=complex(0.0,0.0);
			  in >> ecAmplitude;
			  subPoint->AddECAmplitude(k,ecm,ecAmplitude);
			}
		      }
		      for(EPointMapIterator mappedPoint=point->GetMappedPoints().begin();
			  mappedPoint<point->GetMappedPoints().begin();mappedPoint++) {
			(*mappedPoint)->AddECAmplitude(k,ecm,point->GetECAmplitude(k,ecm));
			for(int i=1;i<=point->NumSubPoints();i++) {
			  (*mappedPoint)->GetSubPoint(i)->
			    AddECAmplitude(k,ecm,point->GetSubPoint(i)->GetECAmplitude(k,ecm));
			}
		      }
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
  if(out.is_open()) {
    out.flush();
    out.close();
  }
  if(in.is_open()) in.close();
}

/*!
 * This function determined what points should be mapped to another to reduce
 * redundant calculations at like energies.
 */

void EData::MapData() {
  for(ESegmentIterator segment=GetSegments().end()-1; 
      segment>=GetSegments().begin(); segment--) {
    for(EPointIterator point=segment->GetPoints().end()-1;
	point>=segment->GetPoints().begin();point--) {
      if(point->NumLocalMappedPoints()==0) {
	for(ESegmentIterator testSegment=GetSegments().begin();
	    testSegment<GetSegments().end(); testSegment++) {
	  if(testSegment->GetEntranceKey()==segment->GetEntranceKey()&&
	     testSegment->GetExitKey()==segment->GetExitKey()) {
	    for(EPointIterator testPoint=testSegment->GetPoints().begin();
		testPoint<testSegment->GetPoints().end();testPoint++) {
	      if(testPoint->GetCMEnergy()==point->GetCMEnergy()
		 &&!testPoint->IsMapped()&&point!=testPoint
		 &&testPoint->GetTargetEffectNum()==point->GetTargetEffectNum()) {
		point->SetMap(testSegment-GetSegments().begin()+1,
			      testPoint-testSegment->GetPoints().begin()+1);
		testPoint->AddLocalMappedPoint(&*point);
		break;
	      }
	    }
	    if(point->IsMapped()) break;
	  }
	}
      }
    }
  }
}

/*!
 * Adds a TargetEffect object to the vector contained within the present object.
 */

void EData::AddTargetEffect(TargetEffect targetEffect) {
  targetEffects_.push_back(targetEffect);
}

/*!
 * Sets the normalization parameter offset in the parameter vector.
 */  

void EData::SetNormParamOffset(int offset) {
  normParamOffset_=offset;
}

/*!
 * Fills the Minuit parameter array from initial values in the EData object.
 */

void EData::FillMnParams(ROOT::Minuit2::MnUserParameters &p) {
  SetNormParamOffset(p.Params().size());
  char varname[50];
  for(ESegmentIterator segment=GetSegments().begin();segment<GetSegments().end();segment++) {
    if(segment->IsVaryNorm()) {
      sprintf(varname,"segment_%d_norm",segment->GetSegmentKey());
      p.Add(varname,segment->GetNorm(),segment->GetNorm()*0.05);
    }
  }
}


/*!
 * Fills the Normalizations from the Minuit parameter array.
 */

void EData::FillNormsFromParams(const vector_r &p) {
  int i=GetNormParamOffset();
  for(ESegmentIterator segment=GetSegments().begin();segment<GetSegments().end();segment++) {
    if(segment->IsVaryNorm()) {
      segment->SetNorm(p[i]); 
      i++;
    }
  }
}

/*!
 * Returns a pointer to a segment specified by a position in the ESegment vector.
 */

ESegment *EData::GetSegment(int segmentNum) {
  ESegment *b=&segments_[segmentNum-1];
  return b;
}

/*!
 * Returns a pointer to a segment based on the segment key, as opposed to a position in the ESegment vector.
 */

ESegment *EData::GetSegmentFromKey(int segmentKey) {
  int segmentNumber=1;
  while(segmentNumber <= this->NumSegments()) {
    if(segmentKey==this->GetSegment(segmentNumber)->GetSegmentKey())
      break;
    else segmentNumber++;
  }
  if(segmentNumber<= this->NumSegments()) return this->GetSegment(segmentNumber);
  else return NULL;
}

/*!
 * Creates a new copy of the EData object in memory and returns a pointer to the new object.
 * Used in AZURECalc function class for thread safety.
 */

EData *EData::Clone() const {
  EData *dataCopy = new EData(*this);
  
  for(EDataIterator data=dataCopy->begin();data!=dataCopy->end();data++) {
    data.point()->SetParentData(dataCopy);
    data.point()->ClearLocalMappedPoints();
  }
  for(EDataIterator data=dataCopy->begin();data!=dataCopy->end();data++) {
    if(data.point()->IsMapped()) {
      EnergyMap pointMap = data.point()->GetMap();
      dataCopy->GetSegment(pointMap.segment)->GetPoint(pointMap.point)->AddLocalMappedPoint(&*data.point());
    }
  }  
  return dataCopy;
}

/*!
 * Returns a pointer to the specified TargetEffect object.
 */

TargetEffect *EData::GetTargetEffect(int effectNumber) {
  TargetEffect *temp;
  if(effectNumber<=targetEffects_.size()) 
    temp=&targetEffects_[effectNumber-1];
  else return temp=NULL;
  return temp;
}

/*!
 * Returns an EDataIterator referring to the first data point in the set.
 */

EDataIterator EData::begin() {
  return EDataIterator(&segments_);
}

/*!
 * Returns an EDataIterator referring to one object past the last data point in the set.
 */

EDataIterator EData::end() {
  EDataIterator it(&segments_);
  return it.SetEnd();
}

/*!
 * Returns a reference to the vector of ESegment objects.
 */

std::vector<ESegment>& EData::GetSegments() {
  return segments_;
}
