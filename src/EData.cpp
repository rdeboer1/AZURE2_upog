#include "AZUREOutput.h"
#include "CNuc.h"
#include "Config.h"
#include "EData.h"
#include "ExtrapLine.h"
#include "SegLine.h"
#include <iostream>
#include <iomanip>

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
  std::ifstream in(configure.segfile.c_str());
  if(!in) {
    return -1;
  }
  std::string dummy;
  getline(in,dummy);
  int numTotalSegments=0;
  while(!in.eof()) {
    SegLine line=ReadSegLine(in);
    if(!in.eof()) {
      numTotalSegments++;
      if(line.isActive==1) {
	ESegment NewSegment(line);
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
  in.close();
  this->ReadTargetEffectsFile(configure.targeteffectsfile);
  this->MapData();
  return 0; 
}

/*!
 * If the AZURE calculation is not data driven, this function is called in place
 * of the EData::Fill function to create points at specified energies and angles.
 * Returns -1 if the input files could not be read, otherwise returns 0.
 */

int EData::MakePoints(const struct Config& configure, CNuc *theCNuc) {
  std::ifstream in(configure.extrapfile.c_str());
  if(!in) {
    return -1;
  }
  std::string dummy;
  getline(in,dummy);
  while(!in.eof()) {
    ExtrapLine line=ReadExtrapLine(in);
    if(line.isActive==1) {
      if(!in.eof()) {
	ESegment NewSegment(line);
	if(theCNuc->IsPairKey(NewSegment.GetEntranceKey())) {
	  if(theCNuc->IsPairKey(NewSegment.GetExitKey())) {
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
  in.close();
  this->ReadTargetEffectsFile(configure.targeteffectsfile);
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
  this->CalcEDependentValues(compound);
  if(configure.checkpene=="screen"||
     configure.checkpene=="file") this->PrintEDependentValues(configure,compound);

  //Calculate legendre polynomials for each data point
  std::cout << "Calculating Legendre Polynomials..." << std::endl;
  this->CalcLegendreP(configure.maxLOrder);
  if(configure.checklegpoly=="screen"||
     configure.checklegpoly=="file") this->PrintLegendreP(configure);

  //Calculate Coulomb Amplitudes
  std::cout << "Calculating Coulomb Amplitudes..." << std::endl;
  this->CalcCoulombAmplitude(compound);
  if(configure.checkcoulamp=="screen"|| configure.checkcoulamp=="file") {
    this->PrintCoulombAmplitude(configure,compound);
  }

  //Calculate new ec amplitudes
  if(configure.isEC) {
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
  if(configure.checkdata=="file") {
    std::string outfile=configure.checkdir+"data.chk";
    fbuffer.open(outfile.c_str(),std::ios::out);
    sbuffer = &fbuffer;
  } else if(configure.checkdata=="screen") sbuffer = std::cout.rdbuf();
  std::ostream out(sbuffer);
  if((configure.checkdata=="file"&&fbuffer.is_open())||configure.checkdata=="screen") {
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
      out << std::setw(11) << GetSegments().begin()-segment+1
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
      out << std::setw(11) << GetSegments().begin()-data.segment()+1
	  << std::setw(14) << (data.segment()->GetPoints()).begin()-data.point()+1
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
for(EDataIterator it=this->begin(); it!=this->end(); it++)
  it.point()->CalcLegendreP(maxL);
}

/*!
 * Prints the Legendre polynomials for each point in the EData object.
 */ 

void EData::PrintLegendreP(const struct Config &configure) {
  std::streambuf *sbuffer;
  std::filebuf fbuffer;
  if(configure.checklegpoly=="file") {
    std::string outfile=configure.checkdir+"legendre.chk";
    fbuffer.open(outfile.c_str(),std::ios::out);
    sbuffer = &fbuffer;
  } else if(configure.checklegpoly=="screen") sbuffer = std::cout.rdbuf();
  std::ostream out(sbuffer);
  if((configure.checklegpoly=="file"&&fbuffer.is_open())||configure.checklegpoly=="screen") {
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
	out << std::setw(10) << GetSegments().begin()-data.segment()+1
	    << std::setw(10) << (data.segment()->GetPoints()).begin()-data.point()+1
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

void EData::CalcEDependentValues(CNuc *theCNuc) {
  for(EDataIterator it=this->begin(); it!=this->end(); it++) 
    if(!(it.point()->IsMapped())) it.point()->CalcEDependentValues(theCNuc);
}

/*!
 * Prints the values calculated by EPoint::CalcEDependentValues for each point in the entire EData object.
 */

void EData::PrintEDependentValues(const struct Config &configure,CNuc *theCNuc) {
  std::streambuf *sbuffer;
  std::filebuf fbuffer;
  if(configure.checkpene=="file") {
    std::string outfile=configure.checkdir+"lomatrixandpene.chk";
    fbuffer.open(outfile.c_str(),std::ios::out);
    sbuffer = &fbuffer;
  } else if(configure.checkpene=="screen") sbuffer = std::cout.rdbuf();
  std::ostream out(sbuffer);
  if((configure.checkpene=="file"&&fbuffer.is_open())||configure.checkpene=="screen") {
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
	    out << std::setw(10) << GetSegments().begin()-data.segment()+1
		<< std::setw(10) << (data.segment()->GetPoints()).begin()-data.point()+1
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
  for(EDataIterator it=this->begin(); it!=this->end(); it++)
    it.point()->CalcCoulombAmplitude(theCNuc);
}

/*!
 * Prints the values calculated by EPoint::CalcCoulombAmplitude for each point in the entire EData object.
 */

void EData::PrintCoulombAmplitude(const struct Config &configure,CNuc *theCNuc) {
  std::streambuf *sbuffer;
  std::filebuf fbuffer;
  if(configure.checkcoulamp=="file") {
    std::string outfile=configure.checkdir+"coulombamplitudes.chk";
    fbuffer.open(outfile.c_str(),std::ios::out);
    sbuffer = &fbuffer;
  } else if(configure.checkcoulamp=="screen") sbuffer = std::cout.rdbuf();
  std::ostream out(sbuffer);
  if((configure.checkcoulamp=="file"&&fbuffer.is_open())||configure.checkcoulamp=="screen") {
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
	  out << std::setw(10) << GetSegments().begin()-segment+1
	      << std::setw(10) << segment->GetPoints().begin()-point+1
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

void EData::WriteOutputFiles(const struct Config &configure) {
  AZUREOutput output(configure.outputdir);
  if(!configure.withData) output.SetExtrap();
  for(ESegmentIterator segment=GetSegments().begin();
      segment<GetSegments().end();segment++) {
    int aa=segment->GetEntranceKey();
    int ir=segment->GetExitKey();
    std::ostream out(output(aa,ir));
    for(EPointIterator point=segment->GetPoints().begin();point<segment->GetPoints().end();point++) {
      out.precision(4);
      out << std::setw(9)  << std::fixed      << point->GetCMEnergy()
	  << std::setw(10) << std::fixed      << point->GetCMAngle()
	  << std::setw(13) << std::scientific << point->GetFitCrossSection()
	  << std::setw(13) << std::scientific << point->GetFitCrossSection()*point->GetSFactorConversion();
      if(!output.IsExtrap()) {
      out << std::setw(13) << std::scientific << point->GetCMCrossSection()
	  << std::setw(13) << std::scientific << point->GetCMCrossSectionError()
	  << std::setw(13) << std::scientific << point->GetCMCrossSection()*point->GetSFactorConversion()
	  << std::setw(13) << std::scientific << point->GetCMCrossSectionError()*point->GetSFactorConversion()
	  << std::endl;
      } else out << std::endl;
    }
    out<<std::endl<<std::endl;out.flush();
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
  if(configure.withData) outputfile=configure.outputdir+"intEC.dat";
  else outputfile=configure.outputdir+"intEC.extrap";
  if(configure.oldECFile) in.open(configure.integralsfile.c_str());
  else {
    out.open(outputfile.c_str());
    if(!out) std::cout << "Could not write to EC Amplitude File." << std::endl;
  }
  for(ESegmentIterator segment=GetSegments().begin();segment<GetSegments().end();segment++) {
    int aa=theCNuc->GetPairNumFromKey(segment->GetEntranceKey());
    if(theCNuc->GetPair(aa)->IsECEntrance()) {
      PPair *entrancePair=theCNuc->GetPair(aa);
      for(int ec=1;ec<=entrancePair->NumECLevels();ec++) {
	int ir=theCNuc->GetPairNumFromKey(segment->GetExitKey());
	if(entrancePair->GetECLevel(ec)->GetPairNum()==ir) {
	  if(!configure.oldECFile) {
	    std::cout << "\tSegment #" << segment->GetSegmentKey() << " [                         ] 0%";std::cout.flush();
	    double percent=0.05;
	    double dpercent=0.05;
	    int numPoints=segment->NumPoints();
	    EPointIterator firstPoint=segment->GetPoints().begin();
	    for(EPointIterator point=firstPoint;point<segment->GetPoints().end();point++) {
	      int pointIndex=point-firstPoint+1;
	      if(pointIndex<=percent*numPoints&&pointIndex+1>percent*numPoints) {
		std::string progress=" [";
		for(int j = 1;j<=25;j++) {
		  if(percent>=j*0.04) progress+='*';
		  else progress+=' ';
		} progress+="] ";
		std::cout << "\r\tSegment #" << segment->GetSegmentKey() << progress << percent*100 << '%';std::cout.flush();
		percent+=dpercent;
	      } else if(pointIndex>percent*numPoints) {
		while(pointIndex>percent*numPoints&&percent<1.) percent+=dpercent;
		std::string progress=" [";
		for(int j = 1;j<=25;j++) {
		  if(percent>=j*0.04) progress+='*';
		  else progress+=' ';
		} progress+="] ";
		std::cout << "\r\tSegment #" << segment->GetSegmentKey() << progress << percent*100 << '%';std::cout.flush();
		percent+=dpercent;
	      }
	      if(!(point->IsMapped())) point->CalculateECAmplitudes(theCNuc);
	    }
	    std::cout << std::endl;
	  }
	  for(EPointIterator point=segment->GetPoints().begin();
	      point<segment->GetPoints().end();point++) {
	    if(!(point->IsMapped())) {
	      for(int k=1;k<=entrancePair->GetDecay(ir)->NumKGroups();k++) {
		for(int ecm=1;ecm<=entrancePair->GetDecay(ir)->GetKGroup(k)->NumECMGroups();ecm++) {
		  if(!configure.oldECFile) {
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
		point->SetMap(GetSegments().begin()-testSegment+1,
			      segment->GetPoints().begin()-testPoint+1);
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
 * Reads the target effects input file and creates the TargetEffect objects 
 * to be applied to the data.
 */

void EData::ReadTargetEffectsFile(std::string infile) {
  std::ifstream in(infile.c_str());
  if(!in) std::cout << "Could not read target effects file." << std::endl;
  else {
    while(!in.eof()) {
      TargetEffect targetEffect(in);
      if(!in.eof()&&targetEffect.IsActive()) {
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
  for(ESegmentIterator segment=GetSegments().begin();segment<GetSegments().end();segment++) {
    if(segment->IsTargetEffect()) {
      TargetEffect *targetEffect = this->GetTargetEffect(segment->GetTargetEffectNum());
      for(EPointIterator point=segment->GetPoints().begin();point<segment->GetPoints().end();point++) {
	point->SetTargetEffectNum(segment->GetTargetEffectNum());
	double forwardDepth=0.0;
	double backwardDepth=0.0;
	if(targetEffect->IsTargetIntegration()) {
	  double targetThickness = targetEffect->TargetThickness(point->GetCMEnergy());
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
	  	double stoppingPower=targetEffect->
	  	GetStoppingPowerEq()->Evaluate(subEnergy);
  	    subPoint.SetStoppingPower(stoppingPower);
	  }
	  point->AddSubPoint(subPoint);
	}
      }
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

EDataIterator EData::begin() {
  return EDataIterator(&segments_);
}

EDataIterator EData::end() {
  EDataIterator it(&segments_);
  return it.SetEnd();
}

std::vector<ESegment>& EData::GetSegments() {
  return segments_;
}
