#include "EData.h"

/*!
 * The EData object has a private attribute containing the number of iterations needed to find the best fit parameters.
 * At creation, this attribute is set to 0.
 */

EData::EData() {
  iterations_=0;
  isFit_=true;
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

int EData::Fill(std::string infile, CNuc *theCNuc) {
  std::ifstream in(infile.c_str());
  if(!in) {
    return -1;
  }
  std::string dummy;
  getline(in,dummy);
  while(!in.eof()) {
    SegLine line=ReadSegLine(in);
    if(!in.eof()) {
      if(line.isActive==1) {
	ESegment NewSegment(line);
	if(theCNuc->IsPairKey(NewSegment.GetEntranceKey())) {
	  theCNuc->GetPair(theCNuc->GetPairNumFromKey(NewSegment.GetEntranceKey()))->SetEntrance();
	  if(theCNuc->IsPairKey(NewSegment.GetExitKey())) {
	    this->AddSegment(NewSegment);
	    if(this->GetSegment(this->NumSegments())->Fill(theCNuc)==-1) {
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
  this->MapData();
  return 0; 
}

/*!
 * If the AZURE calculation is not data driven, this function is called in place
 * of the EData::Fill function to create points at specified energies and angles.
 * Returns -1 if the input files could not be read, otherwise returns 0.
 */

int EData::MakePoints(std::string infile, CNuc *theCNuc) {
  std::ifstream in(infile.c_str());
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
 * Returns true if the data is to be fit, otherwise returns false.  Used in the AZURECalc function class
 * to determine if a clone of the CNuc and EData objects should be made for thread safety.
 */

bool EData::IsFit() const {
  return isFit_;
}

/*!
 * Sets the boolean indicating if the data is to be fit by AZURECalc function class. Used in AZUREMain function
 * class before calls to Minuit and AZURECalc.
 */

void EData::SetFit(bool fit) {
  isFit_=fit;
}

/*!
 * This function updates the number of fit iterations per iteration during the fitting process.
 */

void EData::Iterate(){
  iterations_++;
}

/*!
 * This function is identical in role to the EPoint::Initialize function, except that it initializes
 * and entire EData object instead of a single EPoint object.
 */

void EData::Initialize(CNuc *compound,const Config &configure) {
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

void EData::PrintData(const Config &configure) {
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
	<< std::setw(17) << "Entrance Key #"
	<< std::setw(13) << "Exit Key #"
	<< std::setw(12) << "Min Energy"
	<< std::setw(12) << "Max Energy"
	<< std::setw(11) << "Min Angle"
	<< std::setw(11) << "Max Angle"
	<< std::setw(25) << "Data File"
	<< std::endl; 
    for(int i=1;i<=this->NumSegments();i++) {
      out << std::setw(11) << i
	  << std::setw(17) << this->GetSegment(i)->GetEntranceKey()
	  << std::setw(13) << this->GetSegment(i)->GetExitKey()
	  << std::setw(12) << this->GetSegment(i)->GetMinEnergy()
	  << std::setw(12) << this->GetSegment(i)->GetMaxEnergy()
	  << std::setw(11) << this->GetSegment(i)->GetMinAngle()
	  << std::setw(11) << this->GetSegment(i)->GetMaxAngle()
	  << std::setw(25) << this->GetSegment(i)->GetDataFile()
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
	<< std::endl;
    for(int i=1;i<=this->NumSegments();i++) {
      for(int ii=1;ii<=this->GetSegment(i)->NumPoints();ii++) {
	out << std::setw(11) << i 
	    << std::setw(14) << ii
	    << std::setw(15) << this->GetSegment(i)->GetPoint(ii)->GetLabEnergy() 
	    << std::setw(15) << this->GetSegment(i)->GetPoint(ii)->GetCMEnergy() 
	    << std::setw(15) << this->GetSegment(i)->GetPoint(ii)->GetCMAngle()
	    << std::setw(20) << this->GetSegment(i)->GetPoint(ii)->GetCMCrossSection()
	    << std::setw(22) << this->GetSegment(i)->GetPoint(ii)->GetCMCrossSectionError();
	if(this->GetSegment(i)->GetPoint(ii)->IsMapped()){
	  EnergyMap map=this->GetSegment(i)->GetPoint(ii)->GetMap();
	  char tempMap[25];
	  sprintf(tempMap,"(%d,%d)",map.segment,map.point);
	  out << std::setw(12) <<  tempMap << std::endl;
	} else
	  out << std::setw(12) << "Not Mapped" << std::endl;
      }
      out << std::endl;
    }
  } else std::cout << "Could not write data check file." << std::endl;
  out.flush();
  if(fbuffer.is_open()) fbuffer.close();
}

/*!
 * Calls EPoint::CalcLegendreP for each point in the entire EData object.
 */

void EData::CalcLegendreP(int maxL) {
  for(int i=1;i<=this->NumSegments();i++) {
    for(int ii=1;ii<=this->GetSegment(i)->NumPoints();ii++) {
      EPoint *point=this->GetSegment(i)->GetPoint(ii);
      point->CalcLegendreP(maxL);
    }
  }
}

/*!
 * Prints the Legendre polynomials for each point in the EData object.
 */ 

void EData::PrintLegendreP(const Config &configure) {
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
    for(int i=1;i<=this->NumSegments();i++) {
      for(int ii=1;ii<=this->GetSegment(i)->NumPoints();ii++) {
        EPoint *point=this->GetSegment(i)->GetPoint(ii);
        for(int lOrder=0;lOrder<=point->GetMaxLOrder();lOrder++) {
          out << std::setw(10) << i
          << std::setw(10) << ii
          << std::setw(15) << point->GetCMEnergy()
          << std::setw(15) << point->GetCMAngle()
          << std::setw(5)  << lOrder
          << std::setw(15) << point->GetLegendreP(lOrder) << std::endl;	
        }
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
  for(int i=1;i<=this->NumSegments();i++) {
    ESegment *segment=this->GetSegment(i);
    for(int ii=1;ii<=this->GetSegment(i)->NumPoints();ii++) {
      EPoint *point=this->GetSegment(i)->GetPoint(ii);
      if(!point->IsMapped()) point->CalcEDependentValues(theCNuc);
    }
  }
}

/*!
 * Prints the values calculated by EPoint::CalcEDependentValues for each point in the entire EData object.
 */

void EData::PrintEDependentValues(const Config &configure,CNuc *theCNuc) {
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
    for(int i=1;i<=this->NumSegments();i++) {
      ESegment *segment=this->GetSegment(i);
      for(int ii=1;ii<=this->GetSegment(i)->NumPoints();ii++) {
        EPoint *point=this->GetSegment(i)->GetPoint(ii);
        double inEnergy=point->GetCMEnergy()
        +theCNuc->GetPair(theCNuc->GetPairNumFromKey(segment->GetEntranceKey()))->GetSepE();
        for(int j=1;j<=theCNuc->NumJGroups();j++) {
	  if(theCNuc->GetJGroup(j)->IsInRMatrix()) {
	    JGroup *theJGroup=theCNuc->GetJGroup(j);
	    for(int ch=1;ch<=theJGroup->NumChannels();ch++) {
	      AChannel *theChannel=theJGroup->GetChannel(ch);
	      PPair *thePair=theCNuc->GetPair(theChannel->GetPairNum());
	      int lValue=theChannel->GetL();
	      double localEnergy=inEnergy-thePair->GetSepE()-thePair->GetExE();
	      out << std::setw(10) << i 
		  << std::setw(10) << ii 
		  << std::setw(5) << j 
		  << std::setw(5) << ch  
		  << std::setw(5) << lValue 
		  << std::setw(15) << localEnergy 
		  << std::setw(15) << point->GetSqrtPenetrability(j,ch)
		  << std::setw(25) << point->GetLoElement(j,ch) << std::endl;
	    }           
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
  for(int i=1;i<=this->NumSegments();i++) {
    for(int ii=1;ii<=this->GetSegment(i)->NumPoints();ii++) {
      EPoint *point=this->GetSegment(i)->GetPoint(ii);
      point->CalcCoulombAmplitude(theCNuc);
    }
  }
}

/*!
 * Prints the values calculated by EPoint::CalcCoulombAmplitude for each point in the entire EData object.
 */

void EData::PrintCoulombAmplitude(const Config &configure,CNuc *theCNuc) {
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
    for(int i=1;i<=this->NumSegments();i++) {
      if(this->GetSegment(i)->GetEntranceKey()==this->GetSegment(i)->GetExitKey()) {
	for(int ii=1;ii<=this->GetSegment(i)->NumPoints();ii++) {
	  EPoint *point=this->GetSegment(i)->GetPoint(ii);
	  out << std::setw(10) << i
	      << std::setw(10) << ii
	      << std::setw(10) << theCNuc->GetPairNumFromKey(this->GetSegment(i)->GetEntranceKey())
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

void EData::WriteOutputFiles(const Config &configure) {
  AZUREOutput output(configure.outputdir);
  if(!configure.withData) output.SetExtrap();
  for(int i=1;i<=this->NumSegments();i++) {
    int aa=this->GetSegment(i)->GetEntranceKey();
    int ir=this->GetSegment(i)->GetExitKey();
    std::ostream out(output(aa,ir));
    for(int ii=1;ii<=this->GetSegment(i)->NumPoints();ii++) {
      EPoint *point=this->GetSegment(i)->GetPoint(ii);
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

void EData::CalculateECAmplitudes(CNuc *theCNuc,const Config& configure) {
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
  for(int i=1;i<=this->NumSegments();i++) {
    ESegment *segment=this->GetSegment(i);
    int aa=theCNuc->GetPairNumFromKey(segment->GetEntranceKey());
    if(theCNuc->GetPair(aa)->IsECEntrance()) {
      PPair *entrancePair=theCNuc->GetPair(aa);
      for(int ec=1;ec<=entrancePair->NumECLevels();ec++) {
	int ir=theCNuc->GetPairNumFromKey(this->GetSegment(i)->GetExitKey());
	if(entrancePair->GetECLevel(ec)->GetPairNum()==ir) {
	  if(!configure.oldECFile) {
	    std::cout << "\tSegment #" << i << std::endl;
	    double percent=0.1;
	    double dpercent=0.1;
	    for(int ii=1;ii<=this->GetSegment(i)->NumPoints();ii++) {
	      if(ii<=percent*this->GetSegment(i)->NumPoints()&&
		 ii+1>percent*this->GetSegment(i)->NumPoints()) {
		std::cout << "\t\t" << percent*100 << "%" << std::endl;
		percent+=dpercent;
	      } else if(ii>=percent*this->GetSegment(i)->NumPoints()) {
		while(ii>=percent*this->GetSegment(i)->NumPoints()&&
		      percent<=1.) percent+=dpercent;
		std::cout << "\t\t" << percent*100 << "%" << std::endl;
		percent+=dpercent;
	      }
	      if(!segment->GetPoint(ii)->IsMapped()) segment->GetPoint(ii)->CalculateECAmplitudes(theCNuc);
	    }
	  }
	  for(int ii=1;ii<=this->GetSegment(i)->NumPoints();ii++) {
	    if(!segment->GetPoint(ii)->IsMapped()) {
	      for(int k=1;k<=entrancePair->GetDecay(ir)->NumKGroups();k++) {
		for(int ecm=1;ecm<=entrancePair->GetDecay(ir)->GetKGroup(k)->NumECMGroups();ecm++) {
		  if(!configure.oldECFile) {
		    if(out.is_open()) out << segment->GetPoint(ii)->GetECAmplitude(k,ecm) << std::endl;
		  } else {
		    complex ecAmplitude(0.0,0.0);
		    in >> ecAmplitude;
		    segment->GetPoint(ii)->AddECAmplitude(k,ecm,ecAmplitude);
		  }
		  for(int iii=1;iii<=segment->GetPoint(ii)->NumLocalMappedPoints();iii++) {
		    segment->GetPoint(ii)->GetLocalMappedPoint(iii)->
		      AddECAmplitude(k,ecm,segment->GetPoint(ii)->GetECAmplitude(k,ecm));   
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
  for(int i=this->NumSegments(); i>=1; i--) {
    ESegment *segment=this->GetSegment(i);
    for(int ii=segment->NumPoints();ii>=1;ii--) {
      EPoint *point=segment->GetPoint(ii);
      if(point->NumLocalMappedPoints()==0) {
	for(int j=1;j<=this->NumSegments(); j++) {
	  ESegment *testSegment=this->GetSegment(j);
	  if(testSegment->GetEntranceKey()==segment->GetEntranceKey()&&
	     testSegment->GetExitKey()==segment->GetExitKey()) {
	    for(int jj=1;jj<=testSegment->NumPoints();jj++) {
	      EPoint *testPoint=testSegment->GetPoint(jj);
	      if(testPoint->GetCMEnergy()==point->GetCMEnergy()
		 &&!testPoint->IsMapped()&&point!=testPoint) {
		point->SetMap(j,jj);
		testPoint->AddLocalMappedPoint(point);
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
 * Returns a pointer to a segment specified by a position in the ESegment vector.
 */

ESegment *EData::GetSegment(int segmentNum) {
  ESegment *b=&segments_[segmentNum-1];
  return b;
}

/*!
 * Creates a new copy of the EData object in memory and returns a pointer to the new object.
 * Used in AZURECalc function class for thread safety.
 */

EData *EData::Clone() const {
  EData *dataCopy = new EData(*this);
  
  for(int i=1;i<=dataCopy->NumSegments();i++) {
    for(int ii=1;ii<=dataCopy->GetSegment(i)->NumPoints();ii++) {
      dataCopy->GetSegment(i)->GetPoint(ii)->ClearLocalMappedPoints();
    }
  }
  for(int i=1;i<=dataCopy->NumSegments();i++) {
    for(int ii=1;ii<=dataCopy->GetSegment(i)->NumPoints();ii++) {
      if(dataCopy->GetSegment(i)->GetPoint(ii)->IsMapped()) {
	EPoint *mappedPoint = dataCopy->GetSegment(i)->GetPoint(ii);
	EnergyMap pointMap = mappedPoint->GetMap();
	dataCopy->GetSegment(pointMap.segment)->GetPoint(pointMap.point)->AddLocalMappedPoint(mappedPoint);
      }
    }
  }  
  return dataCopy;
}
