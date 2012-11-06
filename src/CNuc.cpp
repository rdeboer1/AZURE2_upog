#include <iostream>
#include <iomanip>
#include <sstream>
#include "AngCoeff.h"
#include "CNuc.h"
#include "Config.h"
#include "CoulFunc.h"
#include "EigenFunc.h"
#include "ECIntegral.h"
#include "NucLine.h"
#include "Minuit2/MnUserParameters.h"
#include "NFIntegral.h"
#include "ShftFunc.h"

/*!
 * Returns true if a specified pair key exists in the PPair vector, otherwise returns false.
 */

bool CNuc::IsPairKey(int key) {
  bool b=false;
  int c=0;
  while(!b&&c<this->NumPairs()) {
    if(key==this->GetPair(c+1)->GetPairKey()) b=true;
    c++;
  }
  return b;
}

/*!
 * Returns the number of particle pairs in the PPair vector.
 */

int CNuc::NumPairs() const {
  return pairs_.size();
}

/*!
 * Returns the number of \f$ J^\pi \f$ groups in the JGroup vector.
 */

int CNuc::NumJGroups() const {
  return jgroups_.size();
}

/*!
 * Tests if a particle pair exists in the PPair vector. If pair exists, the position in 
 * the vector is returned.  Otherwise, the function returns 0.
 */

int CNuc::IsPair(PPair pair) {
  bool b=false;
  int c=0;
  while(!b&&c<this->NumPairs())
    {
      if(pair.GetPairKey()==this->GetPair(c+1)->GetPairKey()) b=true;
      c++;
    }
  if(b) return c;
  else return 0;
}

/*!
 * Tests if a \f$ J^\pi \f$ group exists in the JGroup vector.  If the group exists, the position in the vector
 * is returned.  Otherwise, the function returns 0.
 */

int CNuc::IsJGroup(JGroup jGroup) {
  bool b=false;
  int c=0;
  while(!b&&c<this->NumJGroups())
    {
      if(jGroup.GetJ()==this->GetJGroup(c+1)->GetJ()&&
	 jGroup.GetPi()==this->GetJGroup(c+1)->GetPi()) b=true;
	 c++;
    }
  if(b) return c;
  else return 0;
}

/*!
 * Returns the position of a particle pair in the PPair vector based on the pair key. 
 * Pair keys are how particle pairs are specified in 
 * the setup files, but may not correspond to the position of the particle pair in the PPair vector.
 * If the pair exists, the position in the vector is returned.  Otherwise, the function returns 0.
 */

int CNuc::GetPairNumFromKey(int key) {
  bool b=false;
  int c=0;
  while(!b&&c<this->NumPairs()) {
    if(key==this->GetPair(c+1)->GetPairKey()) b=true;
    c++;
  }
  if(b) return c;
  else return 0;
}

/*!
 * Fills the compound nucleus object, and all nested objects, from data specified in the nuclear and external capture 
 * input files.  Returns -1 if the files could not be read, and 0 if the files were read successfully.
 */

int CNuc::Fill(const Config &configure) {
  int PairNum,LevelNum,ChannelNum,JGroupNum;
  int maxLValue=0;
  std::ifstream in(configure.configfile.c_str());
  if(!in) return -1;
  std::string line = "";
  while(line!="<levels>"&&!in.eof()) getline(in,line);
  if(line!="<levels>") return -1;
  std::map<int,int> ecPairs;
  line="";
  while(!in.eof()&&line!="</levels>") {
    getline(in,line);
    bool empty=true;
    for(unsigned int i=0;i<line.size();++i) 
      if(line[i]!=' '&&line[i]!='\t') {
	empty=false;
	break;
      }
    if(empty==true) continue;
    if(!in.eof()&&line!="</levels>") {
      std::istringstream stm;
      stm.str(line);
      NucLine Line(stm);
      if(stm.rdstate() & (std::stringstream::failbit | std::stringstream::badbit)) return -1;
      if(Line.l()>maxLValue&&Line.pType()==0) maxLValue=Line.l();
      if(Line.isActive()==1) {
	PPair NewPair(Line);
	PairNum=this->IsPair(NewPair);
	if(!PairNum) {
	  this->AddPair(NewPair);
	  PairNum=this->IsPair(NewPair);
	}
	if(Line.ecMultMask()!=0) {
	  std::map<int,int>::iterator it = ecPairs.find(PairNum);
	  if(it==ecPairs.end()) ecPairs[PairNum]=Line.ecMultMask();
	}
	JGroup NewJGroup(Line);
	JGroupNum=this->IsJGroup(NewJGroup);
	if(!JGroupNum) {
	  this->AddJGroup(NewJGroup);
	  JGroupNum=this->IsJGroup(NewJGroup);
	}
	AChannel NewChannel(Line,PairNum);
	ChannelNum=this->GetJGroup(JGroupNum)->IsChannel(NewChannel);
	if(!ChannelNum) {
	  this->GetJGroup(JGroupNum)->AddChannel(NewChannel);
	  ChannelNum=this->GetJGroup(JGroupNum)->IsChannel(NewChannel);
	  if(this->GetJGroup(JGroupNum)->GetChannel(ChannelNum)->GetL()>maxLValue&&
	     this->GetJGroup(JGroupNum)->GetChannel(ChannelNum)->GetRadType()=='P')
	    maxLValue=this->GetJGroup(JGroupNum)->GetChannel(ChannelNum)->GetL();
	}
	ALevel NewLevel(Line);
	LevelNum=this->GetJGroup(JGroupNum)->IsLevel(NewLevel);
	if(!LevelNum) {
	  this->GetJGroup(JGroupNum)->AddLevel(NewLevel);
	  LevelNum=this->GetJGroup(JGroupNum)->IsLevel(NewLevel);
	}
	this->GetJGroup(JGroupNum)->GetLevel(LevelNum)->AddGamma(Line);
      }
    }
  }
  
  if(line!="</levels>") return -1;

  in.close();

  this->SetMaxLValue(maxLValue);
  if((configure.paramMask & Config::USE_EXTERNAL_CAPTURE) &&
     this->NumJGroups()>0 && this->NumPairs()>0)
    this->ParseExternalCapture(configure,ecPairs);
  
  return 0;
}

/*!
 * Fills the ECLevel vector from information in the external capture file.  Also tests if the final state for external capture
 * exists from the nuclear file.  If not, the state is created.  
 */

void CNuc::ParseExternalCapture(const Config& configure,std::map<int,int>& ecPairs) {
  for(std::map<int,int>::iterator ec=ecPairs.begin();ec!=ecPairs.end();ec++) {
    PPair *exitPair=this->GetPair(ec->first);
    if(exitPair->GetPType()!=10) {
      configure.outStream << "Final state is not a capture pair." << std::endl;
      continue;
    }
    //create new level in compound nucleus for EC state, if it doesn't exist
    double jValue=exitPair->GetJ(2);
    int parity=exitPair->GetPi(2);
    JGroup newJGroup(jValue,parity);
    int jGroupNum=this->IsJGroup(newJGroup);
    int levelNum=0;
    if(jGroupNum) {
      ALevel newLevel(exitPair->GetExE());
      levelNum=this->GetJGroup(jGroupNum)->IsLevel(newLevel);
      if(!levelNum) {
	this->GetJGroup(jGroupNum)->AddLevel(newLevel);
	levelNum=this->GetJGroup(jGroupNum)->IsLevel(newLevel);
	for(int ch=1;ch<=this->GetJGroup(jGroupNum)->NumChannels();ch++) {
	  if(this->GetJGroup(jGroupNum)->GetChannel(ch)->GetRadType()=='P')
	    this->GetJGroup(jGroupNum)->GetLevel(levelNum)->AddGamma(0.1);
	  else this->GetJGroup(jGroupNum)->GetLevel(levelNum)->AddGamma(0.0);
	}
      }
      this->GetJGroup(jGroupNum)->GetLevel(levelNum)->SetECParams(ec->first,ec->second);
      for(int ch=1;ch<=this->GetJGroup(jGroupNum)->NumChannels();ch++) {
	PPair *theFinalPair=this->GetPair(this->GetJGroup(jGroupNum)->GetChannel(ch)->GetPairNum());
	double nfIntegralValue=0.; 
	double ecConvert=0.;
	if(theFinalPair->GetPType()==0) {
	  NFIntegral newNFIntegral(theFinalPair);
	  nfIntegralValue=newNFIntegral(this->GetJGroup(jGroupNum)->GetChannel(ch)->GetL(),exitPair->GetExE());
	  WhitFunc newWhitFunc(theFinalPair);
	  double whitConv=newWhitFunc(this->GetJGroup(jGroupNum)->GetChannel(ch)->GetL(),
				      theFinalPair->GetChRad(),
				      fabs(exitPair->GetExE()-theFinalPair->GetSepE()-theFinalPair->GetExE()));
	  ecConvert=sqrt(2.0*theFinalPair->GetRedMass()*theFinalPair->GetChRad()*uconv/pow(hbarc,2.0))/whitConv;
	}
	this->GetJGroup(jGroupNum)->GetLevel(levelNum)->AddNFIntegral(nfIntegralValue);
	this->GetJGroup(jGroupNum)->GetLevel(levelNum)->AddECConversionFactor(ecConvert);
      }
    } else {
      this->AddJGroup(newJGroup);
      jGroupNum=this->IsJGroup(newJGroup);
      ALevel newLevel(exitPair->GetExE());
      this->GetJGroup(jGroupNum)->AddLevel(newLevel);
      levelNum=this->GetJGroup(jGroupNum)->IsLevel(newLevel);
      this->GetJGroup(jGroupNum)->GetLevel(levelNum)->SetECParams(ec->first,ec->second);
      for(int ir=1;ir<=this->NumPairs();ir++) {
	if(this->GetPair(ir)->GetPType()==0) {
	  double s1=this->GetPair(ir)->GetJ(1);
	  double s2=this->GetPair(ir)->GetJ(2);
	  int sPi=this->GetPair(ir)->GetPi(1)*this->GetPair(ir)->GetPi(2);
	  for(double chS=fabs(s1-s2);chS<=s1+s2;chS+=1.) {
	    for(int chL=0;chL<=this->GetMaxLValue();chL++) {
	      int chPi=sPi*(int)pow(-1,chL);
	      if(fabs(chS-chL)<=jValue&&jValue<=chS+chL&&chPi==parity) {
		AChannel newChannel(chL,chS,ir,'P');
		this->GetJGroup(jGroupNum)->AddChannel(newChannel);
		this->GetJGroup(jGroupNum)->GetLevel(levelNum)->AddGamma(0.1);
		NFIntegral newNFIntegral(this->GetPair(ir));
		double nfIntegralValue=newNFIntegral(chL,exitPair->GetExE());
		WhitFunc newWhitFunc(this->GetPair(ir));
		double whitConv=newWhitFunc(chL,this->GetPair(ir)->GetChRad(),
					    fabs(exitPair->GetExE()-this->GetPair(ir)->GetSepE()-this->GetPair(ir)->GetExE()));
		double ecConvert=sqrt(2.0*this->GetPair(ir)->GetRedMass()*this->GetPair(ir)->GetChRad()*uconv/pow(hbarc,2.0))/whitConv;
		this->GetJGroup(jGroupNum)->GetLevel(levelNum)->AddNFIntegral(nfIntegralValue);
		this->GetJGroup(jGroupNum)->GetLevel(levelNum)->AddECConversionFactor(ecConvert);
		
	      }
	    }
	  }
	}
      }
    }
  }
}

/*!
 * Returns the maximum value of orbital angular momentum read from channels in the nuclear file.
 */

int CNuc::GetMaxLValue() const {
  return maxLValue_;
}

/*!
 * Initializes the compound nucleus object.  This includes calculating the boundary conditions, transforming from
 * physical to formal parameters, creating and sorting all reaction pathways, and calculating angular interference 
 * contributions and coefficients.  A CNuc object can only be initialized for use AFTER it is filled.
 */

void CNuc::Initialize(const Config &configure) {
  //Calculate Boundary Conditions
  configure.outStream << "Calculating Boundary Conditions..." << std::endl;
  this->CalcBoundaryConditions(configure);
  if((configure.fileCheckMask|configure.screenCheckMask) & Config::CHECK_BOUNDARY_CONDITIONS)
    this->PrintBoundaryConditions(configure);

  //Transform Input Parameters
  if(configure.paramMask & Config::TRANSFORM_PARAMETERS) {
    configure.outStream << "Performing Input Parameter Transformation..." << std::endl;
    this->TransformIn(configure);
  }

  //Sort reaction pathways
  configure.outStream << "Sorting Reaction Pathways..." << std::endl;
  this->SortPathways(configure);
  if((configure.fileCheckMask|configure.screenCheckMask) & Config::CHECK_PATHWAYS) 
    this->PrintPathways(configure);
  
  //Calculate Angular Distribution Coefficients
  configure.outStream << "Calculating Angular Distribution Coefficients..." << std::endl;
  this->CalcAngularDists(configure.maxLOrder);
  if((configure.fileCheckMask|configure.screenCheckMask) & Config::CHECK_ANGULAR_DISTS) 
    this->PrintAngularDists(configure);
  
}

/*!
 * Adds a particle pair to the PPair vector.
 */

void CNuc::AddPair(PPair pPair) {
  pairs_.push_back(pPair);
}

/*!
 * Adds a \f$ J^\pi \f$ group to the JGroup vector.
 */

void CNuc::AddJGroup(JGroup jGroup) {
  jgroups_.push_back(jGroup);
}

/*!
 * Prints the initial structure of the compound nucleus object after filling but before initialization.
 * This includes all particle pairs, \f$ J^\pi \f$ groups, levels and channels.
 */

void CNuc::PrintNuc(const Config &configure) {
  std::streambuf *sbuffer;
  std::filebuf fbuffer;
  if(configure.fileCheckMask & Config::CHECK_COMPOUND_NUCLEUS) {
    std::string outfile=configure.checkdir+"compoundnucleus.chk";
    fbuffer.open(outfile.c_str(),std::ios::out);
    sbuffer = &fbuffer;
  } else if(configure.screenCheckMask & Config::CHECK_COMPOUND_NUCLEUS) 
    sbuffer = configure.outStream.rdbuf();
  std::ostream out(sbuffer);
  if(((configure.fileCheckMask & Config::CHECK_COMPOUND_NUCLEUS)&&
      fbuffer.is_open())||
     (configure.screenCheckMask & Config::CHECK_COMPOUND_NUCLEUS)) {
    out << std::endl
	<< "************************************" << std::endl
	<< "*          Particle Pairs          *" << std::endl
	<< "************************************" << std::endl;
    for(int i=1;i<=this->NumPairs();i++) {
      out << "Pair Number: " << i << "  Pair Key: " << this->GetPair(i)->GetPairKey() << std::endl;
      out << std::setw(30) << "Light Particle J: " << this->GetPair(i)->GetJ(1) << std::endl
	  << std::setw(30) << "Light Particle Parity: " << this->GetPair(i)->GetPi(1) << std::endl
	  << std::setw(30) << "Light Particle Z: " << this->GetPair(i)->GetZ(1) << std::endl
	  << std::setw(30) << "Light Particle M: " << this->GetPair(i)->GetM(1) << std::endl
	  << std::setw(30) << "Light Particle g: " << this->GetPair(i)->GetG(1) << std::endl
	  << std::setw(30) << "Heavy Particle J: " << this->GetPair(i)->GetJ(2) << std::endl
	  << std::setw(30) << "Heavy Particle Parity: " << this->GetPair(i)->GetPi(2) << std::endl
	  << std::setw(30) << "Heavy Particle Z: " << this->GetPair(i)->GetZ(2) << std::endl
	  << std::setw(30) << "Heavy Particle M: " << this->GetPair(i)->GetM(2) << std::endl
	  << std::setw(30) << "Heavy Particle g: " << this->GetPair(i)->GetG(2) << std::endl
	  << std::setw(30) << "Seperation Energy [MeV]: " << this->GetPair(i)->GetSepE() << std::endl
	  << std::setw(30) << "Excitation Energy [MeV]: " << this->GetPair(i)->GetExE() << std::endl
	  << std::setw(30) << "Channel Radius: " << this->GetPair(i)->GetChRad() << std::endl;
      if(this->GetPair(i)->GetPType()==0) out << std::setw(30) << "Pair Type: " << "Particle,Particle" << std::endl;
      else if(this->GetPair(i)->GetPType()==10) out << std::setw(30) << "Pair Type: " << "Particle,Gamma" << std::endl;
      else if(this->GetPair(i)->GetPType()==20) out << std::setw(30) << "Pair Type: " << "Beta Decay" << std::endl;
      else out << std::setw(30) << "Pair Type: Unknown" << std::endl;
    }
    out << std::endl
	<< "************************************" << std::endl
	<< "*              Levels              *" << std::endl
	<< "************************************" << std::endl
	<< std::setw(11) << "J Group #" 
	<< std::setw(5)  <<  "J" 
	<< std::setw(4)  << "Pi" 
	<< std::setw(9)  << "Level #" 
	<< std::setw(14) << "Energy [MeV]" 
	<< std::setw(11) << "Channel #" 
	<< std::setw(3)  << "l" 
	<< std::setw(5)  << "s" 
	<< std::setw(8)  << "Pair #" 
	<< std::setw(11)  << "Width" 
	<< std::setw(11) << "Rad. Type" << std::endl;

    for(int i=1;i<=this->NumJGroups();i++) {
      for(int ii=1;ii<=this->GetJGroup(i)->NumLevels();ii++) {
	for(int iii=1;iii<=this->GetJGroup(i)->NumChannels();iii++) {
	  out << std::setw(11) << i 
	      << std::setw(5)  << this->GetJGroup(i)->GetJ()
	      << std::setw(4)  << this->GetJGroup(i)->GetPi()
	      << std::setw(9)  << ii 
	      << std::setw(14) << this->GetJGroup(i)->GetLevel(ii)->GetE()
	      << std::setw(11) << iii
	      << std::setw(3)  << this->GetJGroup(i)->GetChannel(iii)->GetL()
	      << std::setw(5)  << this->GetJGroup(i)->GetChannel(iii)->GetS()
	      << std::setw(8)  << this->GetJGroup(i)->GetChannel(iii)->GetPairNum()
	      << std::setw(11)  << this->GetJGroup(i)->GetLevel(ii)->GetGamma(iii)
	      << std::setw(11) << this->GetJGroup(i)->GetChannel(iii)->GetRadType() << std::endl;
	}
      }
      out << std::endl;
    }
  } else configure.outStream << "Could not write compound nucleus check file." << std::endl;
  out.flush();
  if(fbuffer.is_open()) fbuffer.close();
}

/*!
 * Performs the initial parameter transformations from physical to formal parameters.
 */

void CNuc::TransformIn(const Config& configure) {
  for(int j=1;j<=this->NumJGroups();j++) {
    JGroup *theJGroup=this->GetJGroup(j);
    if(theJGroup->IsInRMatrix()) {
      for(int la=1;la<=theJGroup->NumLevels();la++) {
	ALevel *theLevel=theJGroup->GetLevel(la);
	if(theLevel->IsInRMatrix()) {
	  vector_r tempGammas;
	  std::vector<bool> isNegative;
	  vector_r penes;
	  double denom=2.0;
	  for(int ch=1;ch<=theJGroup->NumChannels();ch++) {
	    AChannel *theChannel=theJGroup->GetChannel(ch);
	    double localEnergy=theLevel->GetE()-this->GetPair(theChannel->GetPairNum())->GetExE()
	      -this->GetPair(theChannel->GetPairNum())->GetSepE();
	    double radius=this->GetPair(theChannel->GetPairNum())->GetChRad();
	    if(theChannel->GetRadType()=='P') {
	      if(localEnergy>0.0) {
		if(theLevel->GetGamma(ch)<0.0) isNegative.push_back(true);
		else isNegative.push_back(false);
		tempGammas.push_back(fabs(theLevel->GetGamma(ch))/1e6);
		CoulFunc theCoulombFunction(this->GetPair(theChannel->GetPairNum()),
					    !!(configure.paramMask&Config::USE_GSL_COULOMB_FUNC));
		double tempPene=theCoulombFunction.Penetrability(theChannel->GetL(),
								 radius,
								 localEnergy);
		denom-=tempGammas[ch-1]/tempPene*
		  theCoulombFunction.PEShift_dE(theChannel->GetL(),radius,localEnergy);
		penes.push_back(tempPene);
	      } else {	     
		if(theLevel->GetGamma(ch)<0.0) isNegative.push_back(true);
		else isNegative.push_back(false);
		tempGammas.push_back(pow(theLevel->GetGamma(ch),2.0));
		ShftFunc theShiftFunction(this->GetPair(theChannel->GetPairNum()));
		WhitFunc newWhitFunc(this->GetPair(theChannel->GetPairNum()));
		double whitConv=newWhitFunc(theChannel->GetL(),radius,fabs(localEnergy));
		double tempPene=this->GetPair(theChannel->GetPairNum())->GetRedMass()*radius*uconv/
		  pow(hbarc,2.0)/pow(whitConv,2.0);
		denom-=tempGammas[ch-1]/tempPene*
		  theShiftFunction.EnergyDerivative(theChannel->GetL(),theLevel->GetE());
		penes.push_back(tempPene);
		}
	    } else if(theChannel->GetRadType()=='E'||theChannel->GetRadType()=='M') {
	      if(fabs(theLevel->GetE()-this->GetPair(theChannel->GetPairNum())->GetExE())<1.e-3&&
		 theJGroup->GetJ()==this->GetPair(theChannel->GetPairNum())->GetJ(2)&&
		 theJGroup->GetPi()==this->GetPair(theChannel->GetPairNum())->GetPi(2)) {
		int tempSign;
		if(theLevel->GetGamma(ch)<0) tempSign=-1;
		else tempSign=1;
		double jValue=theJGroup->GetJ();
		if(int(2.*jValue)%2!=0) tempSign=-tempSign;
		double tempPene=1e-10;
		double tempGamma=theLevel->GetGamma(ch);
		if(theChannel->GetRadType()=='M'&&theChannel->GetL()==1) {
		  tempPene=3.0*jValue/4.0/(jValue+1.);
		} else if(theChannel->GetRadType()=='E'&&theChannel->GetL()==2) {
		  tempPene=60.0*jValue*(2.*jValue-1.)/(jValue+1.)/(2.*jValue+3.);
		  tempGamma=tempGamma*100*sqrt(fstruc*hbarc);
		}
		tempGammas.push_back(pow(tempGamma,2.0));
		penes.push_back(tempPene);
		if(tempSign<0) isNegative.push_back(true);
		else isNegative.push_back(false);
	      } else {
		if(theLevel->GetGamma(ch)<0.0) isNegative.push_back(true);
		else isNegative.push_back(false);
		tempGammas.push_back(fabs(theLevel->GetGamma(ch))/1e6);
		double tempPene = (configure.paramMask & Config::USE_RMC_FORMALISM) ? 1.0 : pow(fabs(localEnergy)/hbarc,2.0*theChannel->GetL()+1);
		if(tempPene<1e-10) tempPene=1e-10;
		penes.push_back(tempPene);
	      }
	    } else if(theChannel->GetRadType()=='F'||theChannel->GetRadType()=='G') {
	      if(theLevel->GetGamma(ch)<0.0) isNegative.push_back(true);
	      else isNegative.push_back(false);
	      tempGammas.push_back(fabs(theLevel->GetGamma(ch)));
	      penes.push_back(1.0);
	    }
	  }
	  if(denom<0.) configure.outStream << "WARNING: Denominator less than zero in E=" 
					   << theLevel->GetE() << " MeV resonance transformation.  "
					   <<  "Tranformation may not have been successful." 
					   << std::endl;
	  double nFSum=1.0;
	  for(int ch=1;ch<=theJGroup->NumChannels();ch++) {
	    AChannel *theChannel=theJGroup->GetChannel(ch);
	    if(theChannel->GetRadType()!='F'&&theChannel->GetRadType()!='G')
	      tempGammas[ch-1]=sqrt(fabs(tempGammas[ch-1]/penes[ch-1]/denom));
	    if(isNegative[ch-1]) tempGammas[ch-1]=-tempGammas[ch-1];
	    theLevel->SetGamma(ch,tempGammas[ch-1]);
	    if(ch<=theLevel->NumNFIntegrals()) nFSum+=2.0*
	      this->GetPair(theChannel->GetPairNum())->GetChRad()*
	      this->GetPair(theChannel->GetPairNum())->GetRedMass()*
	      uconv/pow(hbarc,2.0)*pow(tempGammas[ch-1],2.0)*theLevel->GetNFIntegral(ch);
	  }
	  theLevel->SetSqrtNFFactor(1.0/sqrt(nFSum));
	}
      }
    }
  }
  for(int j=1;j<=this->NumJGroups();j++) {
    JGroup *theJGroup=this->GetJGroup(j);
    if(theJGroup->IsInRMatrix()) {
      std::vector<int> levelKeys;
      vector_r tempEnergies;
      matrix_r tempGammas;
      matrix_r shifts;
      for(int la=1;la<=theJGroup->NumLevels();la++) {
	ALevel *theLevel=theJGroup->GetLevel(la);
	if(theLevel->IsInRMatrix()) {
	  levelKeys.push_back(la);
	  tempEnergies.push_back(theLevel->GetE());
	  vector_r tempChanVector;
	  tempGammas.push_back(tempChanVector);
	  shifts.push_back(tempChanVector);
	  for(int ch=1;ch<=theJGroup->NumChannels();ch++) {
	    AChannel *theChannel=theJGroup->GetChannel(ch);
	    double localEnergy=theLevel->GetE()-this->GetPair(theChannel->GetPairNum())->GetExE()
	      -this->GetPair(theChannel->GetPairNum())->GetSepE();
	    double radius=this->GetPair(theChannel->GetPairNum())->GetChRad();
	    if(theChannel->GetRadType()=='P') {
	      if(localEnergy>0.0) {
		tempGammas[levelKeys.size()-1].push_back(theLevel->GetGamma(ch));
		CoulFunc theCoulombFunction(this->GetPair(theChannel->GetPairNum()),
					    !!(configure.paramMask&Config::USE_GSL_COULOMB_FUNC));
		shifts[levelKeys.size()-1].push_back(theCoulombFunction.PEShift(theChannel->GetL(),
										radius,
										localEnergy));
	      } else {	     
		tempGammas[levelKeys.size()-1].push_back(theLevel->GetGamma(ch));
		ShftFunc theShiftFunction(this->GetPair(theChannel->GetPairNum()));
		shifts[levelKeys.size()-1].push_back(theShiftFunction(theChannel->GetL(),theLevel->GetE()));
		}
	    } else {
	      tempGammas[levelKeys.size()-1].push_back(theLevel->GetGamma(ch));
	      if((theChannel->GetRadType()=='E'||theChannel->GetRadType()=='M') &&
		 (configure.paramMask & Config::USE_EXTERNAL_CAPTURE) && 
		 !(fabs(theLevel->GetGamma(ch))<1.0e-8 && 
		   (configure.paramMask & Config::IGNORE_ZERO_WIDTHS))) {
		complex externalWidth = 
		  CalcExternalWidth(theJGroup,theLevel,theChannel,true,configure);
		if(pow(tempGammas[levelKeys.size()-1][ch-1],2.0)>=pow(imag(externalWidth),2.0)) {
		  if(tempGammas[levelKeys.size()-1][ch-1]<0.0) 
		    tempGammas[levelKeys.size()-1][ch-1]=-sqrt(pow(tempGammas[levelKeys.size()-1][ch-1],2.0)-
							       pow(imag(externalWidth),2.0))-real(externalWidth);
		  else tempGammas[levelKeys.size()-1][ch-1]=sqrt(pow(tempGammas[levelKeys.size()-1][ch-1],2.0)-
								 pow(imag(externalWidth),2.0))-real(externalWidth);
		} else {
		  configure.outStream << "**WARNING: Imaginary portion of external width \n\tfor j=" << j << " la=" 
			    << la << " ch=" << ch << " is greater than total width." << std::endl;
		  tempGammas[levelKeys.size()-1][ch-1]=-real(externalWidth);
		}
	      }
	      shifts[levelKeys.size()-1].push_back(shifts[levelKeys.size()-1][0]);
	    }
	  }
	}
      }	  
      if(!(configure.paramMask & Config::USE_BRUNE_FORMALISM)) {
	matrix_r nMatrix;
	matrix_r mMatrix;      
	for(int mu=0;mu<tempEnergies.size();mu++) {
	  vector_r tempLevelVector;
	  nMatrix.push_back(tempLevelVector);
	  mMatrix.push_back(tempLevelVector);
	  for(int la=0;la<tempEnergies.size();la++) {
	    if(la==mu) {
	      mMatrix[mu].push_back(1.0);
	      double sum=tempEnergies[la];
	      for(int ch=1;ch<=theJGroup->NumChannels();ch++) {
		if(theJGroup->GetChannel(ch)->GetRadType()=='P')
		  sum+=(shifts[la][ch-1]-theJGroup->GetChannel(ch)->GetBoundaryCondition())*
		    pow(tempGammas[la][ch-1],2.0);
	      }
	      nMatrix[mu].push_back(sum);
	    } else {
	      double mSum=0.0;
	      double nSum=0.0;
	      for(int ch=1;ch<=theJGroup->NumChannels();ch++) {
		if(theJGroup->GetChannel(ch)->GetRadType()=='P') {
		  mSum+=(shifts[mu][ch-1]-shifts[la][ch-1])/(tempEnergies[mu]-tempEnergies[la])*
		    tempGammas[la][ch-1]*tempGammas[mu][ch-1];
		  nSum+=((tempEnergies[mu]*shifts[la][ch-1]-tempEnergies[la]*shifts[mu][ch-1])/
			 (tempEnergies[mu]-tempEnergies[la])-theJGroup->GetChannel(ch)->GetBoundaryCondition())
		    *tempGammas[la][ch-1]*tempGammas[mu][ch-1];
		}
	      }
	      mMatrix[mu].push_back(-mSum);
	      nMatrix[mu].push_back(nSum);
	    }
	  }
	}
	//solve eigenvalue problem
	EigenFunc eigenFunc(nMatrix,mMatrix);
	for(int la=0;la<tempEnergies.size();la++) {
	  theJGroup->GetLevel(levelKeys[la])->SetE(eigenFunc.eigenvalues()[la]);
	  for(int ch=1;ch<=theJGroup->NumChannels();ch++) {
	    double sum=0.0;
	    for(int mu=0;mu<tempEnergies.size();mu++) {
              sum+=eigenFunc.eigenvectors()[mu][la]*tempGammas[mu][ch-1];
	    }
	    theJGroup->GetLevel(levelKeys[la])->SetGamma(ch,sum);
	  }
	}
      } else {
	for(int la=0;la<tempEnergies.size();la++) 
	  for(int ch=1;ch<=theJGroup->NumChannels();ch++) 
	    theJGroup->GetLevel(levelKeys[la])->SetGamma(ch,tempGammas[la][ch-1]);
      }  
    }
  }
}

/*!
 * Calculates internal and external reaction pathways.
 */

void CNuc::SortPathways(const Config& configure) {
  int DecayNum, KGroupNum, MGroupNum;
  for(int aa=1;aa<=this->NumPairs();aa++) {
    if(!this->GetPair(aa)->IsEntrance()) continue;
    for(int ir=1;ir<=this->NumPairs();ir++) {
      if(this->GetPair(ir)->GetPType()==20) continue;
      if(this->GetPair(aa)->GetPType()==20) {
	for(int l = 0; l < 2; l++) {
	  for(int j=1;j<=this->NumJGroups();j++) {
	    if(!this->GetJGroup(j)->IsInRMatrix()) continue;	      
	    for(int ch=1;ch<=this->GetJGroup(j)->NumChannels();ch++) {
	      if(this->GetJGroup(j)->GetChannel(ch)->GetPairNum()!=aa) continue;
	      for(int chp=1;chp<=this->GetJGroup(j)->NumChannels();chp++) {
		if(this->GetJGroup(j)->GetChannel(chp)->GetPairNum()!=ir||
		   this->GetJGroup(j)->GetChannel(ch)->GetL()!=l) continue;
		Decay NewDecay(ir);
		DecayNum=this->GetPair(aa)->IsDecay(NewDecay);
		if(!DecayNum) {
		  this->GetPair(aa)->AddDecay(NewDecay);
		  DecayNum=this->GetPair(aa)->IsDecay(NewDecay);		  
		}
		KGroup NewKGroup(l,0);
		KGroupNum=this->GetPair(aa)->GetDecay(DecayNum)->IsKGroup(NewKGroup);
		if(!KGroupNum) {
		  this->GetPair(aa)->GetDecay(DecayNum)->AddKGroup(NewKGroup);
		  KGroupNum=this->GetPair(aa)->GetDecay(DecayNum)->IsKGroup(NewKGroup);
		}
		MGroup NewMGroup(j,ch,chp);
		MGroupNum=this->GetPair(aa)->GetDecay(DecayNum)->GetKGroup(KGroupNum)->IsMGroup(NewMGroup);
		if(!MGroupNum) {
		  this->GetPair(aa)->GetDecay(DecayNum)->GetKGroup(KGroupNum)->AddMGroup(NewMGroup);
		  MGroupNum=this->GetPair(aa)->GetDecay(DecayNum)->GetKGroup(KGroupNum)->IsMGroup(NewMGroup);
		}
	      }
	    }
	  }	  
	}
      } else if(this->GetPair(ir)->GetPType()==0) {
	for(double s=fabs(this->GetPair(aa)->GetJ(1)-this->GetPair(aa)->GetJ(2));
	    s<=(this->GetPair(aa)->GetJ(1)+this->GetPair(aa)->GetJ(2));s+=1.) {
	  for(double sp=fabs(this->GetPair(ir)->GetJ(1)-this->GetPair(ir)->GetJ(2));
	      sp<=(this->GetPair(ir)->GetJ(1)+this->GetPair(ir)->GetJ(2));sp+=1.) {
	    for(int j=1;j<=this->NumJGroups();j++) {
	      if(!this->GetJGroup(j)->IsInRMatrix()) continue;
	      for(int ch=1;ch<=this->GetJGroup(j)->NumChannels();ch++) {
		if(this->GetJGroup(j)->GetChannel(ch)->GetPairNum()!=aa) continue;
		for(int chp=1;chp<=this->GetJGroup(j)->NumChannels();chp++) {
		  if(this->GetJGroup(j)->GetChannel(chp)->GetPairNum()!=ir||
		     this->GetJGroup(j)->GetChannel(ch)->GetS()!=s||
		     this->GetJGroup(j)->GetChannel(chp)->GetS()!=sp) continue;
		  Decay NewDecay(ir);
		  DecayNum=this->GetPair(aa)->IsDecay(NewDecay);
		  if(!DecayNum) {
		    this->GetPair(aa)->AddDecay(NewDecay);
		    DecayNum=this->GetPair(aa)->IsDecay(NewDecay);		  
		  }
		  KGroup NewKGroup(s,sp);
		  KGroupNum=this->GetPair(aa)->GetDecay(DecayNum)->IsKGroup(NewKGroup);
		  if(!KGroupNum) {
		    this->GetPair(aa)->GetDecay(DecayNum)->AddKGroup(NewKGroup);
		    KGroupNum=this->GetPair(aa)->GetDecay(DecayNum)->IsKGroup(NewKGroup);
		  }
		  MGroup NewMGroup(j,ch,chp);
		  MGroupNum=this->GetPair(aa)->GetDecay(DecayNum)->GetKGroup(KGroupNum)->IsMGroup(NewMGroup);
		  if(!MGroupNum) {
		    this->GetPair(aa)->GetDecay(DecayNum)->GetKGroup(KGroupNum)->AddMGroup(NewMGroup);
		    MGroupNum=this->GetPair(aa)->GetDecay(DecayNum)->GetKGroup(KGroupNum)->IsMGroup(NewMGroup);
		  }
		  double statspinfactor=(2.*this->GetJGroup(j)->GetJ()+1.)*
		    this->GetPair(this->GetJGroup(j)->GetChannel(chp)->GetPairNum())->GetI1I2Factor();
		  this->GetPair(aa)->GetDecay(DecayNum)->GetKGroup(KGroupNum)->
		    GetMGroup(MGroupNum)->SetStatSpinFactor(statspinfactor);
		}
	      }
	    }
	  }
	}
      } else if(this->GetPair(ir)->GetPType()==10 && !(configure.paramMask & Config::USE_RMC_FORMALISM)) {
	for(double s=fabs(this->GetPair(aa)->GetJ(1)-this->GetPair(aa)->GetJ(2));
	    s<=(this->GetPair(aa)->GetJ(1)+this->GetPair(aa)->GetJ(2));s+=1.) {
	  for(int j=1;j<=this->NumJGroups();j++) {
	    if(!this->GetJGroup(j)->IsInRMatrix()) continue;
	    for(int ch=1;ch<=this->GetJGroup(j)->NumChannels();ch++) {
	      if(this->GetJGroup(j)->GetChannel(ch)->GetPairNum()!=aa) continue;
	      for(int chp=1;chp<=this->GetJGroup(j)->NumChannels();chp++) {
		if(this->GetJGroup(j)->GetChannel(chp)->GetPairNum()!=ir||
		   this->GetJGroup(j)->GetChannel(ch)->GetS()!=s) continue;
		Decay NewDecay(ir);
		DecayNum=this->GetPair(aa)->IsDecay(NewDecay);
		if(!DecayNum) {
		  this->GetPair(aa)->AddDecay(NewDecay);
		  DecayNum=this->GetPair(aa)->IsDecay(NewDecay);		  
		}
		KGroup NewKGroup(s,0);
		KGroupNum=this->GetPair(aa)->GetDecay(DecayNum)->IsKGroup(NewKGroup);
		if(!KGroupNum) {
		  this->GetPair(aa)->GetDecay(DecayNum)->AddKGroup(NewKGroup);
		  KGroupNum=this->GetPair(aa)->GetDecay(DecayNum)->IsKGroup(NewKGroup);
		}
		MGroup NewMGroup(j,ch,chp);
		MGroupNum=this->GetPair(aa)->GetDecay(DecayNum)->GetKGroup(KGroupNum)->IsMGroup(NewMGroup);
		if(!MGroupNum) {
		  this->GetPair(aa)->GetDecay(DecayNum)->GetKGroup(KGroupNum)->AddMGroup(NewMGroup);
		  MGroupNum=this->GetPair(aa)->GetDecay(DecayNum)->GetKGroup(KGroupNum)->IsMGroup(NewMGroup);
		}		    
		double statspinfactor=(2.*this->GetJGroup(j)->GetJ()+1.)*
		  this->GetPair(this->GetJGroup(j)->GetChannel(chp)->GetPairNum())->GetI1I2Factor();
		this->GetPair(aa)->GetDecay(DecayNum)->GetKGroup(KGroupNum)->
		  GetMGroup(MGroupNum)->SetStatSpinFactor(statspinfactor);
	      }
	    }
	  }
	}
      }
    }
  }
  for(int aa=1;aa<=this->NumPairs();aa++) { //loop over all pairs
    PPair *entrancePair=this->GetPair(aa);
    if(entrancePair->GetPType()==20 || !entrancePair->IsEntrance()) continue;
    for(int j=1;j<=this->NumJGroups();j++) {
      JGroup *theFinalJGroup=this->GetJGroup(j);
      for(int la=1;la<=theFinalJGroup->NumLevels();la++) {
	ALevel *theFinalLevel=theFinalJGroup->GetLevel(la);
	if(!theFinalLevel->IsECLevel()) continue;
	int decayNum=entrancePair->IsDecay(theFinalLevel->GetECPairNum()); //store RESONANCE decay number to final state
	if(!decayNum) continue; //if this is a resonance decay...
	for(int k=1;k<=entrancePair->GetDecay(decayNum)->NumKGroups();k++) { //loop over all kgroups for decays to final state 
	  KGroup *theKGroup=entrancePair->GetDecay(decayNum)->GetKGroup(k);
	  for(int chp=1;chp<=theFinalJGroup->NumChannels();chp++) { //loop over all final configurations in the capture state
	    AChannel *finalChannel=theFinalJGroup->GetChannel(chp);
	    if(this->GetPair(finalChannel->GetPairNum())->GetPType()!=0) continue;  //ensure the configuration is a particle pair
	    int chDecayNum=entrancePair->IsDecay(finalChannel->GetPairNum());
	    if(!chDecayNum) continue; //if it is actually a resonance decay...
	    for(int kp=1;kp<=entrancePair->GetDecay(chDecayNum)->NumKGroups();kp++) { 
	      if(entrancePair->GetDecay(chDecayNum)->GetKGroup(kp)->GetS()!=theKGroup->GetS()) continue;
	      for(int mp=1;mp<=entrancePair->GetDecay(chDecayNum)->GetKGroup(kp)->NumMGroups();mp++) {
		MGroup *chMGroup=entrancePair->GetDecay(chDecayNum)->GetKGroup(kp)->GetMGroup(mp);
		AChannel *chChannel=this->GetJGroup(chMGroup->GetJNum())->GetChannel(chMGroup->GetChNum());
		AChannel *chChannelp=this->GetJGroup(chMGroup->GetJNum())->GetChannel(chMGroup->GetChpNum());
		for(int multL=1;multL<=maxECMult;multL++) { //loop over all allowed gamma parities
		  char radType;
		  if(this->GetJGroup(chMGroup->GetJNum())->GetPi()*theFinalJGroup->GetPi()==(int)pow(-1,multL)) radType='E';
		  else radType='M'; //calculate radiation type
		  if(!((radType=='E' && multL==1) && (theFinalLevel->GetECMultMask()&isE1)) &&
		     !((radType=='M' && multL==1) && (theFinalLevel->GetECMultMask()&isM1)) &&
		     !((radType=='E' && multL==2) && (theFinalLevel->GetECMultMask()&isE2)) ) continue; //allow only m1,e1,e2
		  if(fabs(this->GetJGroup(chMGroup->GetJNum())->GetJ()-multL)>theFinalJGroup->GetJ()||
		     theFinalJGroup->GetJ()>this->GetJGroup(chMGroup->GetJNum())->GetJ()+multL) continue; 
		  if(!(abs(chChannelp->GetL()-multL)<=finalChannel->GetL()&&
		       finalChannel->GetL()<=chChannelp->GetL()+multL&&
		       fabs(chChannelp->GetS()-finalChannel->GetL())<=theFinalJGroup->GetJ()&&
		       theFinalJGroup->GetJ()<=chChannelp->GetS()+finalChannel->GetL()&&
		       chChannelp->GetS()==finalChannel->GetS())&&
		     !(fabs(chChannelp->GetS()-multL)<=finalChannel->GetS()&&
		       finalChannel->GetS()<=chChannelp->GetS()+multL&&
		       fabs(chChannelp->GetL()-finalChannel->GetS())<=theFinalJGroup->GetJ()&&
		       theFinalJGroup->GetJ()<=chChannelp->GetL()+finalChannel->GetS()&&
		       chChannelp->GetL()==finalChannel->GetL()&&
		       radType=='M')) continue; //ensure entrance channel for dc can couple to final state		     		  
		  if(chChannel==chChannelp) {
		    ECMGroup newECMGroup(radType,multL,chChannel->GetL(),
					 this->GetJGroup(chMGroup->GetJNum())->GetJ(),chp,j,la);
		    theKGroup->AddECMGroup(newECMGroup);
		  }
		  int internalChannel=0;
		  for(int intCh=1;intCh<=this->GetJGroup(chMGroup->GetJNum())->NumChannels();intCh++) {
		    if(this->GetJGroup(chMGroup->GetJNum())->GetChannel(intCh)->GetRadType()==radType &&
		       this->GetJGroup(chMGroup->GetJNum())->GetChannel(intCh)->GetL()==multL &&
		       this->GetJGroup(chMGroup->GetJNum())->GetChannel(intCh)->GetPairNum()==theFinalLevel->GetECPairNum()) {
		      internalChannel=intCh;
		      break;
		    }
		  }
		  ECMGroup newECMGroup(radType,multL,chChannel->GetL(),this->GetJGroup(chMGroup->GetJNum())->GetJ(),
				       chp,j,la,chDecayNum,kp,mp,internalChannel);
		  theKGroup->AddECMGroup(newECMGroup);
		}
	      }
	    }
	  }
	}
      }
    }
  }
}

/*!
 * Prints the internal and external reaction pathways.
 */
  
void CNuc::PrintPathways(const Config &configure) {
  std::streambuf *sbuffer;
  std::filebuf fbuffer;
  if(configure.fileCheckMask & Config::CHECK_PATHWAYS) {
    std::string outfile=configure.checkdir+"pathways.chk";
    fbuffer.open(outfile.c_str(),std::ios::out);
    sbuffer = &fbuffer;
  } else if(configure.screenCheckMask & Config::CHECK_PATHWAYS) sbuffer = configure.outStream.rdbuf();
  std::ostream out(sbuffer);
  if(((configure.fileCheckMask & Config::CHECK_PATHWAYS)&&fbuffer.is_open())
     ||(configure.screenCheckMask & Config::CHECK_PATHWAYS)) {   
    out << std::endl
	<< "************************************" << std::endl
	<< "*    Internal Reaction Pathways    *" << std::endl
	<< "************************************" << std::endl
	<< std::setw(17) << "Entrance Pair #" 
	<< std::setw(9)  << "Decay #"
	<< std::setw(14) << "Decay Pair #"
	<< std::setw(12) << "K Group #"
	<< std::setw(16) << "Entrance Ch. s"
	<< std::setw(13) << "Decay Ch. s"
	<< std::setw(11) << "M Group #"
	<< std::setw(11) << "J Group #"
	<< std::setw(16) << "Entrance Ch. #"
	<< std::setw(12) << "Exit Ch. #" << std::endl;
    for(int i=1;i<=this->NumPairs();i++) {
      PPair *thePair=this->GetPair(i);
      for(int ii=1;ii<=this->GetPair(i)->NumDecays();ii++){
	for(int iii=1;iii<=this->GetPair(i)->GetDecay(ii)->NumKGroups();iii++) {
	  for(int iiii=1;iiii<=this->GetPair(i)->GetDecay(ii)->GetKGroup(iii)->NumMGroups();iiii++) {	      
	    out << std::setw(17) << i
		<< std::setw(9)  << ii
		<< std::setw(14) << this->GetPair(i)->GetDecay(ii)->GetPairNum()
		<< std::setw(12) << iii
		<< std::setw(16) << this->GetPair(i)->GetDecay(ii)->GetKGroup(iii)->GetS()
		<< std::setw(13) << this->GetPair(i)->GetDecay(ii)->GetKGroup(iii)->GetSp()
		<< std::setw(11) << iiii
		<< std::setw(11) << this->GetPair(i)->GetDecay(ii)->GetKGroup(iii)->GetMGroup(iiii)->GetJNum()
		<< std::setw(16) << this->GetPair(i)->GetDecay(ii)->GetKGroup(iii)->GetMGroup(iiii)->GetChNum()
		<< std::setw(12) << this->GetPair(i)->GetDecay(ii)->GetKGroup(iii)->GetMGroup(iiii)->GetChpNum() << std::endl;
	  }
	}
	out << std::endl;
      }
    }
    out << std::endl
	<< "************************************" << std::endl
	<< "*    External Reaction Pathways    *" << std::endl
	<< "************************************" << std::endl
	<< std::setw(17) << "Entrance Pair #" 
	<< std::setw(9)  << "Decay #"
	<< std::setw(14) << "Decay Pair #"
	<< std::setw(12) << "K Group #"
	<< std::setw(16) << "Entrance Ch. s"
	<< std::setw(13) << "Decay Ch. s"
	<< std::setw(11) << "M Group #"
	<< std::setw(11) << "Mult."
	<< std::setw(11) << "J_i Value"
	<< std::setw(11) << "J_f Value"
	<< std::setw(11) << "l_i Value"
	<< std::setw(11) << "l_f Value"
	<< std::setw(13) << "Type"
	<< std::setw(13) << "Ch. Decay #" 
	<< std::setw(11) << "Ch. K #" 
	<< std::setw(11) << "Ch. M #" 
	<< std::setw(11) << "Int. Ch #" 
	<< std::endl;
    for(int i=1;i<=this->NumPairs();i++) {
      PPair *thePair=this->GetPair(i);
      for(int ii=1;ii<=this->GetPair(i)->NumDecays();ii++){
	for(int iii=1;iii<=this->GetPair(i)->GetDecay(ii)->NumKGroups();iii++) {
	  for(int iiii=1;iiii<=this->GetPair(i)->GetDecay(ii)->GetKGroup(iii)->NumECMGroups();iiii++) {	 
	    ECMGroup *theECMGroup=this->GetPair(i)->GetDecay(ii)->GetKGroup(iii)->GetECMGroup(iiii);
	    JGroup *theECJGroup=this->GetJGroup(theECMGroup->GetJGroupNum());
	    out << std::setw(17) << i
		<< std::setw(9)  << ii
		<< std::setw(14) << this->GetPair(i)->GetDecay(ii)->GetPairNum()
		<< std::setw(12) << iii
		<< std::setw(16) << this->GetPair(i)->GetDecay(ii)->GetKGroup(iii)->GetS()
		<< std::setw(13) << theECJGroup->GetChannel(theECMGroup->GetFinalChannel())->GetS()
		<< std::setw(11) << iiii
		<< std::setw(10) << theECMGroup->GetRadType() << theECMGroup->GetMult()
		<< std::setw(11) << theECMGroup->GetJ()
		<< std::setw(11) << theECJGroup->GetJ()  	    
		<< std::setw(11) << theECMGroup->GetL()
		<< std::setw(11) << theECJGroup->GetChannel(theECMGroup->GetFinalChannel())->GetL();
	    if(theECMGroup->IsChannelCapture()) out << std::setw(13) << "Channel"
						    << std::setw(13) << theECMGroup->GetChanCapDecay()
						    << std::setw(11) << theECMGroup->GetChanCapKGroup()
						    << std::setw(11) << theECMGroup->GetChanCapMGroup()
						    << std::setw(11) << theECMGroup->GetIntChannelNum() << std::endl;
	    else out << std::setw(15) << "Hard Sphere" << std::endl;
	  }
	}     
	out << std::endl;
      }
    }
  } else configure.outStream << "Could not write pathways check file." << std::endl;
  out.flush();
  if(fbuffer.is_open()) fbuffer.close();
}

/*!
 * Calculates the boundary conditions.  Boundary conditions for each channel are evaluated at the energy of the
 * first level read from the nuclear input file in the \f$ J^\pi \f$ group.
 */

void CNuc::CalcBoundaryConditions(const Config& configure){
  for(int j=1;j<=this->NumJGroups();j++) {
    if(this->GetJGroup(j)->IsInRMatrix()) {
      JGroup *theJGroup=this->GetJGroup(j);
      ALevel *firstLevel=theJGroup->GetLevel(1);
      if(firstLevel->IsInRMatrix()) {
	for(int ch=1;ch<=theJGroup->NumChannels();ch++) {
	  AChannel *theChannel=theJGroup->GetChannel(ch);
	  PPair *thePair=this->GetPair(theChannel->GetPairNum());
	  if(thePair->GetPType()==0) {
	    int lValue=theChannel->GetL();
	    double levelEnergy=firstLevel->GetE();
	    double resonanceEnergy=levelEnergy-(thePair->GetSepE()+thePair->GetExE());
	    if(resonanceEnergy<0.0) {
	      ShftFunc theShiftFunction(thePair);
	      theChannel->SetBoundaryCondition(theShiftFunction(lValue,levelEnergy));
	    }
	    else {
	      CoulFunc theCoulombFunction(thePair,
					  !!(configure.paramMask&Config::USE_GSL_COULOMB_FUNC));
	      double radius=thePair->GetChRad();
	      double boundary=theCoulombFunction.PEShift(lValue,radius,resonanceEnergy);
	      theChannel->SetBoundaryCondition(boundary);
	    }
	  }
	  else {
	    double boundary=theJGroup->GetChannel(1)->GetBoundaryCondition();
	    theChannel->SetBoundaryCondition(boundary);
	  }
	}
      }
    }
  }
}

/*!
 * Prints the boundary conditions.
 */

void CNuc::PrintBoundaryConditions(const Config &configure) {
  std::streambuf *sbuffer;
  std::filebuf fbuffer;
  if(configure.fileCheckMask & Config::CHECK_BOUNDARY_CONDITIONS) {
    std::string outfile=configure.checkdir+"boundaryconditions.chk";
    fbuffer.open(outfile.c_str(),std::ios::out);
    sbuffer = &fbuffer;
  } else if(configure.screenCheckMask & Config::CHECK_BOUNDARY_CONDITIONS) sbuffer = configure.outStream.rdbuf();
  std::ostream out(sbuffer);
  if(((configure.fileCheckMask & Config::CHECK_BOUNDARY_CONDITIONS)&&fbuffer.is_open())||
     (configure.screenCheckMask & Config::CHECK_BOUNDARY_CONDITIONS)) {
    out << std::endl
        << "************************************" << std::endl
        << "*        Boundary Conditions       *" << std::endl
        << "************************************" << std::endl;
    out << std::setw(10) << "J Group #"
        << std::setw(10) << "Channel #"
        << std::setw(20) << "Boundary Condition"
        << std::endl;
    for(int j=1;j<=this->NumJGroups();j++) {
      if(this->GetJGroup(j)->IsInRMatrix()) {
	JGroup *theJGroup=this->GetJGroup(j);
	for (int ch=1;ch<=theJGroup->NumChannels();ch++) {
	  AChannel *theChannel=theJGroup->GetChannel(ch);
	  out << std::setw(10) << j
	      << std::setw(10) << ch
	      << std::setw(20) << theChannel->GetBoundaryCondition()
	      << std::endl;
	}
      }
    }
  } else configure.outStream << "Could not write boundary conditions check file." << std::endl;
  out.flush();
  if(fbuffer.is_open()) fbuffer.close();
}

/*!
 * Creates and sorts the KLGroup and Interference objects and calculates the appropriate coefficients.
 */

void CNuc::CalcAngularDists(int maxL) {
  for(int aa=1;aa<=this->NumPairs();aa++) {
    PPair *entrancePair=this->GetPair(aa);
    if(entrancePair->GetPType()==20) continue;
    for(int ir=1;ir<=this->GetPair(aa)->NumDecays();ir++) {
      Decay *theDecay=this->GetPair(aa)->GetDecay(ir);
      for(int k=1;k<=theDecay->NumKGroups();k++) {
        for(int lOrder=0;lOrder<=maxL;lOrder++) {
          for(int m1=1;m1<=theDecay->GetKGroup(k)->NumMGroups()+theDecay->GetKGroup(k)->NumECMGroups();m1++){
            for(int m2=1;m2<=theDecay->GetKGroup(k)->NumMGroups()+theDecay->GetKGroup(k)->NumECMGroups();m2++) {
	      std::string interferenceType;
	      double j1,j2,l1,l1p,l2,l2p;
	      int w1p,w2p,path1,path2;
	      if(m1>theDecay->GetKGroup(k)->NumMGroups()) {
		int m1_ec=m1-theDecay->GetKGroup(k)->NumMGroups();
		ECMGroup *theECMGroup1=theDecay->GetKGroup(k)->GetECMGroup(m1_ec);
		j1=theECMGroup1->GetJ();
		l1=(double) theECMGroup1->GetL();
		l1p=(double) theECMGroup1->GetMult();
		if(theECMGroup1->GetRadType()=='M') w1p=0;
		else w1p=1;
		interferenceType='E';
		path1=m1_ec;
	      } else {
		JGroup *jgroup1=this->GetJGroup(theDecay->GetKGroup(k)->GetMGroup(m1)->GetJNum());
		AChannel *channel1=jgroup1->GetChannel(theDecay->GetKGroup(k)->GetMGroup(m1)->GetChNum());
		AChannel *channel1p=jgroup1->GetChannel(theDecay->GetKGroup(k)->GetMGroup(m1)->GetChpNum());
		j1=jgroup1->GetJ();
		l1=(double) channel1->GetL();
		l1p=(double) channel1p->GetL();
		if(channel1p->GetRadType()=='M'||channel1p->GetRadType()=='P') w1p=0;
		else w1p=1;
		interferenceType='R';
		path1=m1;
	      }
	      if(m2>theDecay->GetKGroup(k)->NumMGroups()) {
		int m2_ec=m2-theDecay->GetKGroup(k)->NumMGroups();
		ECMGroup *theECMGroup2=theDecay->GetKGroup(k)->GetECMGroup(m2_ec);
		j2=theECMGroup2->GetJ();
		l2=(double) theECMGroup2->GetL();
		l2p=(double) theECMGroup2->GetMult();
		if(theECMGroup2->GetRadType()=='M') w2p=0;
		else w2p=1;
		interferenceType+='E';
		path2=m2_ec;
	      } else {
		JGroup *jgroup2=this->GetJGroup(theDecay->GetKGroup(k)->GetMGroup(m2)->GetJNum());
		AChannel *channel2=jgroup2->GetChannel(theDecay->GetKGroup(k)->GetMGroup(m2)->GetChNum());
		AChannel *channel2p=jgroup2->GetChannel(theDecay->GetKGroup(k)->GetMGroup(m2)->GetChpNum());
		j2=jgroup2->GetJ();
		l2=(double) channel2->GetL();
		l2p=(double) channel2p->GetL();
		if(channel2p->GetRadType()=='M'||channel2p->GetRadType()=='P') w2p=0;
		else w2p=1;
		interferenceType+='R';
		path2=m2;
	      }
	      double s=theDecay->GetKGroup(k)->GetS();
	      double sp=theDecay->GetKGroup(k)->GetSp();
	      if((int)(l1+l2+lOrder)%2==0&&(int)(l1p+l2p+w1p+w2p+lOrder)%2==0) {
		double z1z2=0.0;
		double z1=sqrt(2.*l1+1.)*sqrt(2.*l2+1.)*sqrt(2.*j1+1.)*sqrt(2.*j2+1.)
		  *AngCoeff::ClebGord(l1,l2,lOrder,0.,0.,0.)*AngCoeff::Racah(l1,j1,l2,j2,s,lOrder);
		if(this->GetPair(theDecay->GetPairNum())->GetPType()==0) {
		  double z2=sqrt(2.*l1p+1.)*sqrt(2.*l2p+1.)*sqrt(2.*j1+1.)*sqrt(2.*j2+1.)
		    *AngCoeff::ClebGord(l1p,l2p,lOrder,0.,0.,0.)*AngCoeff::Racah(l1p,j1,l2p,j2,sp,lOrder);
		  z1z2=pow(-1.0,sp-s)/4.*z1*z2;
		} else if(this->GetPair(theDecay->GetPairNum())->GetPType()==10) {
		  double jf=this->GetPair(theDecay->GetPairNum())->GetJ(2);
		  double z2=sqrt(2.*l1p+1.)*sqrt(2.*l2p+1.)*sqrt(2.*j1+1.)*sqrt(2.*j2+1.)
		    *AngCoeff::ClebGord(l1p,l2p,lOrder,1.,-1.,0)*AngCoeff::Racah(l1p,j1,l2p,j2,jf,lOrder);
		  z1z2=pow(-1.,1.+s-jf)/4.*z1*z2;
		}
		if(fabs(z1z2)>1e-10) {
		  KLGroup NewKLGroup(k,lOrder);
		  int KLGroupNum=theDecay->IsKLGroup(NewKLGroup);
		  if(!KLGroupNum) {
		    theDecay->AddKLGroup(NewKLGroup);
		    KLGroupNum=theDecay->IsKLGroup(NewKLGroup);
		  }
		  Interference NewInterference(path1,path2,z1z2,interferenceType);
		  int InterNum=theDecay->GetKLGroup(KLGroupNum)->IsInterference(NewInterference);
		  if(!InterNum) {
		    theDecay->GetKLGroup(KLGroupNum)->AddInterference(NewInterference);
		    InterNum=theDecay->GetKLGroup(KLGroupNum)->IsInterference(NewInterference);
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

/*!
 * Prints the KLGroup and Interference object structure as well as the calculated coefficients.
 */

void CNuc::PrintAngularDists(const Config &configure) {
  std::streambuf *sbuffer;
  std::filebuf fbuffer;
  if(configure.fileCheckMask & Config::CHECK_ANGULAR_DISTS) {
    std::string outfile=configure.checkdir+"angulardistributions.chk";
    fbuffer.open(outfile.c_str(),std::ios::out);
    sbuffer = &fbuffer;
  } else if(configure.screenCheckMask & Config::CHECK_ANGULAR_DISTS) sbuffer = configure.outStream.rdbuf();
  std::ostream out(sbuffer);
  if(((configure.fileCheckMask & Config::CHECK_ANGULAR_DISTS)&&fbuffer.is_open())||
     (configure.screenCheckMask & Config::CHECK_ANGULAR_DISTS)) {
    out << std::endl
        << "************************************" << std::endl
        << "*       Angular Distributions       *" << std::endl
        << "************************************" << std::endl;
    out << std::setw(10) << "ir"
	<< std::setw(10) << "k"
	<< std::setw(10) << "L"
	<< std::setw(10) << "m1" 
	<< std::setw(10) << "m2"
	<< std::setw(10) << "z1z2"
	<< std::setw(10) << "type"
	<< std::endl;
    for(int aa=1;aa<=this->NumPairs();aa++) {
      for(int ir=1;ir<=this->GetPair(aa)->NumDecays();ir++) {
	Decay *theDecay=this->GetPair(aa)->GetDecay(ir);
	for(int kl=1;kl<=theDecay->NumKLGroups();kl++) {
	  KLGroup *theKLGroup=theDecay->GetKLGroup(kl);
	  for(int i=1;i<=theKLGroup->NumInterferences();i++){
	    Interference *theInter=theKLGroup->GetInterference(i);
	    out << std::setw(10) << theDecay->GetPairNum()
		<< std::setw(10) << theKLGroup->GetK()
		<< std::setw(10) << theKLGroup->GetLOrder()
		<< std::setw(10) << theInter->GetM1() 
		<< std::setw(10) << theInter->GetM2()
		<< std::setw(10) << theInter->GetZ1Z2()
		<< std::setw(10) << theInter->GetInterferenceType()
		<< std::endl;
	  }
	  out << std::endl;
	} 
      }
    }
  }  else configure.outStream << "Could not write angular distributions check file." << std::endl;
  out.flush();
  if(fbuffer.is_open()) fbuffer.close();
}

/*!
 * Fills the Minuit parameter array from initial values in the CNuc object.
 */

void CNuc::FillMnParams(ROOT::Minuit2::MnUserParameters &p) {
  char varname[50];
  for(int j=1;j<=this->NumJGroups();j++) {
    for(int la=1;la<=this->GetJGroup(j)->NumLevels();la++) {
      ALevel *level=this->GetJGroup(j)->GetLevel(la);
      sprintf(varname,"j=%d_la=%d_energy",j,la);
      p.Add(varname,level->GetE(),0.1*level->GetE());
      bool isUnbound=false;
      for(int ir=1;ir<=this->NumPairs();ir++) {
	PPair *pair=this->GetPair(ir);
	if(pair->GetPType()==0 &&
	   level->GetE()>(pair->GetSepE()+pair->GetExE())) isUnbound=true;
      }
      if(!isUnbound) p.Fix(varname);
      if(level->EnergyFixed()&&!p.Parameter(p.Index(varname)).IsFixed()) p.Fix(varname); 
      for(int ch=1;ch<=this->GetJGroup(j)->NumChannels();ch++) {
	sprintf(varname,"j=%d_la=%d_ch=%d_rwa",j,la,ch);
	p.Add(varname,level->GetGamma(ch),0.1*level->GetGamma(ch));
	if(level->GetGamma(ch)==0.0) p.Fix(varname);
	if(level->ChannelFixed(ch)&&!p.Parameter(p.Index(varname)).IsFixed()) p.Fix(varname);
      }
    }
  }
}

/*!
 * Fills the CNuc object from the Minuit parameter array.
 */

void CNuc::FillCompoundFromParams(const vector_r &p) {
  int i=0;
  for(int j=1;j<=this->NumJGroups();j++) {
    for(int la=1;la<=this->GetJGroup(j)->NumLevels();la++) {
      ALevel *level=this->GetJGroup(j)->GetLevel(la);
      level->SetFitE(p[i]);i++;
      double nFSum=1.0;
      for(int ch=1;ch<=this->GetJGroup(j)->NumChannels();ch++) {
	level->SetFitGamma(ch,p[i]);
	if(ch<=level->NumNFIntegrals()) nFSum+=2.0*
	  this->GetPair(this->GetJGroup(j)->GetChannel(ch)->GetPairNum())->GetChRad()*
	  this->GetPair(this->GetJGroup(j)->GetChannel(ch)->GetPairNum())->GetRedMass()*
	  uconv/pow(hbarc,2.0)*pow(p[i],2.0)*level->GetNFIntegral(ch);
	i++;
      }
      level->SetSqrtNFFactor(1.0/sqrt(nFSum));
    }
  }
}

/*!
 * Performs the final parameter transformations from formal to physical parameters.
 */

void CNuc::TransformOut(const Config& configure) {
  if(!(configure.paramMask & Config::USE_BRUNE_FORMALISM)) {
    int maxIterations=1000;
    double energyTolerance=1e-6;
    for(int j=1;j<=this->NumJGroups();j++) {
      for(int la=1;la<=this->GetJGroup(j)->NumLevels();la++) {
	ALevel *theLevel=this->GetJGroup(j)->GetLevel(la);
	if(theLevel->IsInRMatrix()) {
	  int iteration=1;
	  int thisLevel=0;
	  bool done=false;
	  vector_r tempE;
	  vector_r tempBoundary;
	  matrix_r tempGamma;
	  for(int lap=1;lap<=this->GetJGroup(j)->NumLevels();lap++) {
	    if(this->GetJGroup(j)->GetLevel(lap)->IsInRMatrix()) {
	      tempE.push_back(this->GetJGroup(j)->GetLevel(lap)->GetFitE());
	      if(this->GetJGroup(j)->GetLevel(lap)==theLevel) thisLevel=tempE.size()-1;
	      vector_r tempChanVector;
	      tempGamma.push_back(tempChanVector);
	      for(int ch=1;ch<=this->GetJGroup(j)->NumChannels();ch++) {
		tempGamma[tempE.size()-1].push_back(this->GetJGroup(j)->GetLevel(lap)->GetFitGamma(ch));
		if(tempE.size()==1) tempBoundary.push_back(this->GetJGroup(j)->GetChannel(ch)->GetBoundaryCondition());
	      }
	    }
	  }
	  while(iteration<=maxIterations&&!done) {
	    vector_r boundaryDiff;
	    for(int ch=1;ch<=this->GetJGroup(j)->NumChannels();ch++) {
	      double newBoundary=0.0;
	      AChannel *theChannel=this->GetJGroup(j)->GetChannel(ch);
	      PPair *exitPair=this->GetPair(theChannel->GetPairNum());
	      double localEnergy=tempE[thisLevel]-exitPair->GetSepE()-exitPair->GetExE();
	      if(theChannel->GetRadType()=='P') {
		if(localEnergy<0.0) {
		  ShftFunc theShiftFunction(exitPair);
		  newBoundary=theShiftFunction(theChannel->GetL(),tempE[thisLevel]);
		}
		else {
		  CoulFunc theCoulombFunction(exitPair,
					      !!(configure.paramMask&Config::USE_GSL_COULOMB_FUNC));
		  double radius=exitPair->GetChRad();
		  newBoundary=theCoulombFunction.PEShift(theChannel->GetL(),radius,localEnergy);
		}
		boundaryDiff.push_back(newBoundary-tempBoundary[ch-1]);
		tempBoundary[ch-1]=newBoundary;
	      } else boundaryDiff.push_back(boundaryDiff[0]);
	    }
	    matrix_r cMatrix;
	    for(int mu=0;mu<tempE.size();mu++) {
	      vector_r tempRow;
	      cMatrix.push_back(tempRow);
	      for (int mup=0;mup<tempE.size();mup++) {
		double chanSum=0.0;
		for(int ch=1;ch<=this->GetJGroup(j)->NumChannels();ch++) {
		  if(this->GetJGroup(j)->GetChannel(ch)->GetRadType()=='P')
		    chanSum+=boundaryDiff[ch-1]*tempGamma[mu][ch-1]*
		      tempGamma[mup][ch-1];
		}
		if(mu==mup) cMatrix[mu].push_back(tempE[mu]-chanSum);
		else cMatrix[mu].push_back(-chanSum);
	      }
	    }
	    EigenFunc eigenFunc(cMatrix);	
	    if(fabs(eigenFunc.eigenvalues()[thisLevel]-tempE[thisLevel])<=energyTolerance) 
	      done=true;
	    matrix_r newGamma;
	    for(int mu=0;mu<tempE.size();mu++) {
	      vector_r tempChanVector;
	      newGamma.push_back(tempChanVector);
	      for(int ch=1;ch<=this->GetJGroup(j)->NumChannels();ch++) {
		double gammaSum=0.0;
		for(int mup=0;mup<tempE.size();mup++) {
		  gammaSum+=eigenFunc.eigenvectors()[mup][mu]*tempGamma[mup][ch-1];
		}
		newGamma[mu].push_back(gammaSum);
	      }
	    }
	    for(int mu=0;mu<tempE.size();mu++) {
	      tempE[mu]=eigenFunc.eigenvalues()[mu];
	      for(int ch=1;ch<=this->GetJGroup(j)->NumChannels();ch++) {
		tempGamma[mu][ch-1]=newGamma[mu][ch-1];
	      }
	    }
	    if(!done) {
	      if(iteration==maxIterations) {
		configure.outStream << "**WARNING: Could Not Transform J = " 
			  << this->GetJGroup(j)->GetJ();
		if(this->GetJGroup(j)->GetPi()==-1) configure.outStream << '-';
		else configure.outStream << '+';
		configure.outStream << " E = " << theLevel->GetFitE() << " MeV**" << std::endl;
		tempE[thisLevel]=theLevel->GetFitE();
		for(int ch=1;ch<=this->GetJGroup(j)->NumChannels();ch++) 
		  tempGamma[thisLevel][ch-1]=theLevel->GetFitGamma(ch);
	      }
	      iteration++;
	    }
	  }
	  
	  theLevel->SetTransformE(tempE[thisLevel]);
	  theLevel->SetTransformIterations(iteration);
	  double nFSum=1.0;
	  for(int ch=1;ch<=this->GetJGroup(j)->NumChannels();ch++) {
	    AChannel *theChannel=this->GetJGroup(j)->GetChannel(ch);
	    theLevel->SetTransformGamma(ch,tempGamma[thisLevel][ch-1]);
	    if(ch<=theLevel->NumNFIntegrals()) nFSum+=2.0*
						 this->GetPair(theChannel->GetPairNum())->GetChRad()*
						 this->GetPair(theChannel->GetPairNum())->GetRedMass()*
						 uconv/pow(hbarc,2.0)*pow(tempGamma[thisLevel][ch-1],2.0)*
						 theLevel->GetNFIntegral(ch);
	  }
	  theLevel->SetSqrtNFFactor(1.0/sqrt(nFSum));
	} else {
	  theLevel->SetTransformE(theLevel->GetFitE());
	  theLevel->SetTransformIterations(0);
	  for(int ch=1;ch<=this->GetJGroup(j)->NumChannels();ch++)
	    theLevel->SetTransformGamma(ch,theLevel->GetFitGamma(ch));	
	}
      } 
    }
  } else {
    for(int j=1;j<=this->NumJGroups();j++) 
      for(int la=1;la<=this->GetJGroup(j)->NumLevels();la++) {
	this->GetJGroup(j)->GetLevel(la)->SetTransformIterations(0);
	this->GetJGroup(j)->GetLevel(la)->SetTransformE(this->GetJGroup(j)->GetLevel(la)->GetFitE());
	for(int ch=1;ch<=this->GetJGroup(j)->NumChannels();ch++)
	  this->GetJGroup(j)->GetLevel(la)->
	    SetTransformGamma(ch,this->GetJGroup(j)->GetLevel(la)->GetFitGamma(ch));	
      }
  }
    
  for(int j=1;j<=this->NumJGroups();j++) {
    JGroup *theJGroup=this->GetJGroup(j);
    for(int la=1;la<=this->GetJGroup(j)->NumLevels();la++) {
      ALevel *theLevel=this->GetJGroup(j)->GetLevel(la);
      double normSum=0.0;
      vector_r tempPene;
      for(int ch=1;ch<=this->GetJGroup(j)->NumChannels();ch++) {
	AChannel *theChannel=this->GetJGroup(j)->GetChannel(ch);
	PPair *exitPair=this->GetPair(theChannel->GetPairNum());
	double localEnergy=theLevel->GetTransformE()-exitPair->GetSepE()-exitPair->GetExE();
	if(theChannel->GetRadType()=='P') {
	  if(localEnergy<0.0) {
	    ShftFunc theShiftFunction(exitPair);
	    normSum+=theShiftFunction.EnergyDerivative(theChannel->GetL(),theLevel->GetTransformE())*
	      pow(theLevel->GetTransformGamma(ch),2.0);
	    WhitFunc newWhitFunc(exitPair);
	    double whitConv=newWhitFunc(theChannel->GetL(),
					exitPair->GetChRad(),
					fabs(localEnergy));
	    double pene=exitPair->GetRedMass()*exitPair->GetChRad()*uconv/
	      pow(hbarc,2.0)/pow(whitConv,2.0);
	    tempPene.push_back(pene);
	  }
	  else {
	    CoulFunc theCoulombFunction(exitPair,
					!!(configure.paramMask&Config::USE_GSL_COULOMB_FUNC));
	    double radius=exitPair->GetChRad();
	    normSum+=theCoulombFunction.PEShift_dE(theChannel->GetL(),radius,localEnergy)*
	      pow(theLevel->GetTransformGamma(ch),2.0);
	    double pene=theCoulombFunction.Penetrability(theChannel->GetL(),radius,localEnergy);
	    tempPene.push_back(pene);
	  }
	} else if(theChannel->GetRadType()=='M'||theChannel->GetRadType()=='E') {
	  if(fabs(theLevel->GetE()-this->GetPair(theChannel->GetPairNum())->GetExE())<1.e-3&&
	     theJGroup->GetJ()==this->GetPair(theChannel->GetPairNum())->GetJ(2)&&
	     theJGroup->GetPi()==this->GetPair(theChannel->GetPairNum())->GetPi(2)) {
	    double jValue=theJGroup->GetJ();
	    double pene=1e-10;
	    if(theChannel->GetRadType()=='M'&&theChannel->GetL()==1)
	      pene=3.0*jValue/4.0/(jValue+1.);
	    else if(theChannel->GetRadType()=='E'&&theChannel->GetL()==2)
	      pene=60.0*jValue*(2.*jValue-1.)/(jValue+1.)/(2.*jValue+3.);
	    if((int)(2*jValue)%2!=0) pene*=-1.;
	    tempPene.push_back(pene);
	  } else {
	    double pene = (configure.paramMask & Config::USE_RMC_FORMALISM) ? 1.0 : pow(fabs(localEnergy)/hbarc,2.0*theChannel->GetL()+1);
	    tempPene.push_back(pene);
	  }
	} else tempPene.push_back(1.0);
      }
      for(int ch=1;ch<=this->GetJGroup(j)->NumChannels();ch++) {
	AChannel* theChannel = this->GetJGroup(j)->GetChannel(ch);
	complex externalWidth(0.0,0.0);
	if((theChannel->GetRadType()=='M'||theChannel->GetRadType()=='E') &&
	   theLevel->IsInRMatrix()&&(configure.paramMask & Config::USE_EXTERNAL_CAPTURE) &&
	   !(fabs(theLevel->GetTransformGamma(ch))<1.0e-8 && (configure.paramMask & Config::IGNORE_ZERO_WIDTHS)))
	  externalWidth=CalcExternalWidth(this->GetJGroup(j),theLevel,
					  this->GetJGroup(j)->GetChannel(ch),false,configure);
	theLevel->SetExternalGamma(ch,externalWidth);
	complex totalWidth=theLevel->GetTransformGamma(ch)+externalWidth;
	int tempSign = (real(totalWidth)<0.) ? (-1) : (1);
	double bigGamma;
	if(theChannel->GetRadType()!='F'&&theChannel->GetRadType()!='G')
	  bigGamma=tempSign*2.0*real(totalWidth*conj(totalWidth))*tempPene[ch-1]/
	    (1.0+normSum);
	else bigGamma=real(totalWidth);
	theLevel->SetBigGamma(ch,bigGamma);
      } 
    }
  } 
}

/*!
 * Writes the final transformed parameters to "parameters.out" file. 
 */

void CNuc::PrintTransformParams(const Config& configure) {
  char filename[256];;
  sprintf(filename,"%sparameters.out",configure.outputdir.c_str());
  std::ofstream out;
  out.open(filename);
  if(out) {
    out << "PHYSICAL LEVEL PARAMETERS (BOUNDARY CONDITION SET TO SHIFT AT E_LEVEL)" << std::endl;
    out << std::endl;
    for(int j=1;j<=this->NumJGroups();j++) {
      JGroup *theJGroup=this->GetJGroup(j);
      for(int la=1;la<=this->GetJGroup(j)->NumLevels();la++) {
	out.precision(1);
	out << std::fixed;
	ALevel *theLevel=this->GetJGroup(j)->GetLevel(la);
	out << "J = " << std::setw(3) << this->GetJGroup(j)->GetJ();
	if(this->GetJGroup(j)->GetPi()==-1) out << '-';
	else out << '+';
	out.precision(4);
	out << "  E_level = " << std::setw(8) << theLevel->GetTransformE() << " MeV" 
	    << "  ITERATIONS = " << std::setw(5) << theLevel->GetTransformIterations() << std::endl;
	for(int ch=1;ch<=this->GetJGroup(j)->NumChannels();ch++) {
	  AChannel *theChannel=this->GetJGroup(j)->GetChannel(ch);
	  PPair *exitPair=this->GetPair(theChannel->GetPairNum());
	  double localEnergy=theLevel->GetTransformE()-exitPair->GetSepE()-exitPair->GetExE();
	  out << "  R = " << std::setw(2) << exitPair->GetPairKey();
	  if(theChannel->GetRadType()=='P') out << "  l = " << std::setw(3) << theChannel->GetL();
	  else if(theChannel->GetRadType()=='F') out << "  Fermi Beta Decay ";
	  else if(theChannel->GetRadType()=='G') out << "   G-T Beta Decay  ";
	  else out << "  L = " << std::setw(2) << theChannel->GetRadType() << theChannel->GetL();
	  out.precision(1);
	  if(theChannel->GetRadType()!='G'&&theChannel->GetRadType()!='F')
	    out << "  s = " << std::setw(4) << theChannel->GetS();
	  out.precision(6);
	  if(localEnergy<0.0&&theChannel->GetRadType()=='P') {
	    out << "  C  = " << std::setw(12) << sqrt(fabs(theLevel->GetBigGamma(ch))) 
		<< " fm^(-1/2)";
	  } else if(fabs(theLevel->GetE()-this->GetPair(theChannel->GetPairNum())->GetExE())<1.e-3&&
	     theJGroup->GetJ()==this->GetPair(theChannel->GetPairNum())->GetJ(2)&&
	     theJGroup->GetPi()==this->GetPair(theChannel->GetPairNum())->GetPi(2)&&
		    theChannel->GetRadType()=='M'&&theChannel->GetL()==1) {
	    int tempSign = (theLevel->GetBigGamma(ch)<0) ? (-1) : (1);
	    out << "  mu = " << std::setw(12) << tempSign*sqrt(fabs(theLevel->GetBigGamma(ch))) 
		<< " nm       ";
	  } else if(fabs(theLevel->GetE()-this->GetPair(theChannel->GetPairNum())->GetExE())<1.e-3&&
	     theJGroup->GetJ()==this->GetPair(theChannel->GetPairNum())->GetJ(2)&&
	     theJGroup->GetPi()==this->GetPair(theChannel->GetPairNum())->GetPi(2)&&
		    theChannel->GetRadType()=='E'&&theChannel->GetL()==2) {
	    int tempSign = (theLevel->GetBigGamma(ch)<0) ? (-1) : (1);
	    out << "  Q  = " << std::setw(12) << tempSign*sqrt(fabs(theLevel->GetBigGamma(ch)))/100.0/sqrt(fstruc*hbarc) 
		<< " b        ";
	  } else if(theChannel->GetRadType()=='F'|| theChannel->GetRadType()=='G') {
	    out << "  B  = " << std::setw(12) << theLevel->GetBigGamma(ch)
		<< "          ";
	  } else {
	    if(fabs(theLevel->GetBigGamma(ch))>=1e-3)
	      out << "  G  = " << std::setw(12) << fabs(theLevel->GetBigGamma(ch))*1e3 
		  << " keV      ";
	    else if(fabs(theLevel->GetBigGamma(ch))>=1e-6)
	      out << "  G  = " << std::setw(12) << fabs(theLevel->GetBigGamma(ch))*1e6 
		  << " eV       ";
	    else
	      out << "  G  = " << std::setw(12) << fabs(theLevel->GetBigGamma(ch))*1e9 
		  << " meV      ";
	  }  
	  out << "  g_int = " << std::setw(12) << theLevel->GetTransformGamma(ch); 
	  if(theChannel->GetRadType()!='G'&&theChannel->GetRadType()!='F') out << " MeV^(1/2) ";
	  else out << "           ";
	  out << "  g_ext = " << std::setw(20) << theLevel->GetExternalGamma(ch);
	  if(theChannel->GetRadType()!='G'&&theChannel->GetRadType()!='F') out << " MeV^(1/2) ";
	  out << std::endl;
	}
	out << std::endl;
      }
    }
  } else configure.outStream << "Could not save parameters.out file." << std::endl;
}

/*!
 * Sets the maximum orbital angular momentum value read from the nuclear input file.
 */

void CNuc::SetMaxLValue(int maxL) {
  maxLValue_=maxL;
}

/*!
 * This function is called for each iteration to calculate the shift 
 * functions at new level energies when the Brune parametrization is used. 
 */

void CNuc::CalcShiftFunctions(const Config& configure) {
  for(int j=1;j<=this->NumJGroups();j++) {
    if(this->GetJGroup(j)->IsInRMatrix()) {
      JGroup *theJGroup=this->GetJGroup(j);
      for(int la=1;la<=theJGroup->NumLevels();la++) {
	ALevel *theLevel=theJGroup->GetLevel(la);
	if(theLevel->IsInRMatrix()) {
	  for(int ch=1;ch<=theJGroup->NumChannels();ch++) {
	    AChannel *theChannel=theJGroup->GetChannel(ch);
	    PPair *thePair=this->GetPair(theChannel->GetPairNum());
	    if(thePair->GetPType()==0) {
	      int lValue=theChannel->GetL();
	      double levelEnergy=theLevel->GetFitE();
	      double resonanceEnergy=levelEnergy-(thePair->GetSepE()+thePair->GetExE());
	      if(resonanceEnergy<0.0) {
		ShftFunc theShiftFunction(thePair);
		theLevel->SetShiftFunction(ch,theShiftFunction(lValue,levelEnergy));
	      }
	      else {
		CoulFunc theCoulombFunction(thePair,
					    !!(configure.paramMask&Config::USE_GSL_COULOMB_FUNC));
		double radius=thePair->GetChRad();
		theLevel->SetShiftFunction(ch,theCoulombFunction.PEShift(lValue,radius,resonanceEnergy));
	      }
	    }
	    else {
	      theLevel->SetShiftFunction(ch,theJGroup->GetLevel(1)->GetShiftFunction(1));
	    }
	  }
	}
      }
    }
  }
}

/*!
 * Calculates the external reduced width amplitudes for a given channel.
 */

complex CNuc::CalcExternalWidth(JGroup* theJGroup, ALevel* theLevel, 
				AChannel *theChannel,bool isInitial,const Config& configure) {
  complex externalWidth(0.0,0.0);
  if(theChannel->GetRadType()=='E'||(theChannel->GetRadType()=='M'&&theChannel->GetL()==1)) {
    bool isExternal=false;
    int j=0;
    int la=0;
    while(!isExternal&&j<this->NumJGroups()) {
      j++;
      la=0;
      while(!isExternal&&la<this->GetJGroup(j)->NumLevels()) {
	la++;
	if(this->GetJGroup(j)->GetLevel(la)->IsECLevel()&&
	   theChannel->GetPairNum()==this->GetJGroup(j)->GetLevel(la)->GetECPairNum()) {
	  isExternal=true;
	}
      }
    }
    if(isExternal) {
      JGroup *theFinalJGroup=this->GetJGroup(j);
      ALevel *theFinalLevel=theFinalJGroup->GetLevel(la);
      double theLevelEnergy;
      if(!isInitial) theLevelEnergy=theLevel->GetTransformE();
      else theLevelEnergy=theLevel->GetE();
      int multL=theChannel->GetL();
      if(((theChannel->GetRadType()=='E' && multL==1) && (theFinalLevel->GetECMultMask()&isE1)) ||
	 ((theChannel->GetRadType()=='M' && multL==1) && (theFinalLevel->GetECMultMask()&isM1)) ||
	 ((theChannel->GetRadType()=='E' && multL==2) && (theFinalLevel->GetECMultMask()&isE2)) ) { //allow only m1,e1,e2
	double theFinalLevelEnergy;
	if(!isInitial) theFinalLevelEnergy=theFinalLevel->GetTransformE();
	else theFinalLevelEnergy=theFinalLevel->GetE();
	for(int ch=1;ch<=theJGroup->NumChannels();ch++) {
	  double theInitialChannelGamma;
	  if(!isInitial) theInitialChannelGamma=theLevel->GetTransformGamma(ch);
	  else theInitialChannelGamma=theLevel->GetGamma(ch);
	  AChannel *initialChannel=theJGroup->GetChannel(ch);
	  if(initialChannel->GetRadType()=='P') {
	    for(int chp=1;chp<=theFinalJGroup->NumChannels();chp++) {
	      double theFinalChannelGamma;
	      if(!isInitial) theFinalChannelGamma=theFinalLevel->GetTransformGamma(chp);
	      else theFinalChannelGamma=theFinalLevel->GetGamma(chp);
	      AChannel *finalChannel=theFinalJGroup->GetChannel(chp);
	      if(finalChannel->GetRadType()=='P') {
		if(finalChannel->GetPairNum()==initialChannel->GetPairNum()) {
		  if((abs(initialChannel->GetL()-multL)<=finalChannel->GetL()&&finalChannel->GetL()<=initialChannel->GetL()+multL&&
		      fabs(initialChannel->GetS()-finalChannel->GetL())<=theFinalJGroup->GetJ()&&
		      theFinalJGroup->GetJ()<=initialChannel->GetS()+finalChannel->GetL()&&initialChannel->GetS()==finalChannel->GetS())||
		     (fabs(initialChannel->GetS()-multL)<=finalChannel->GetS()&&finalChannel->GetS()<=initialChannel->GetS()+multL&&
		      fabs(initialChannel->GetL()-finalChannel->GetS())<=theFinalJGroup->GetJ()&&
		      theFinalJGroup->GetJ()<=initialChannel->GetL()+finalChannel->GetS()&&initialChannel->GetL()==finalChannel->GetL()&&
		      theChannel->GetRadType()=='M')) {
		    PPair *theFinalPair=this->GetPair(finalChannel->GetPairNum());
		    
		    ECIntegral theECIntegral(theFinalPair,configure);
		    complex integrals = theECIntegral(initialChannel->GetL(),finalChannel->GetL(),
						      initialChannel->GetS(),finalChannel->GetS(),
						      theJGroup->GetJ(),theFinalJGroup->GetJ(),
						      multL,theChannel->GetRadType(),
						      theLevelEnergy,theFinalLevelEnergy,
						      true);
		    
		    double ecNormParam=theFinalChannelGamma*
		      theFinalLevel->GetSqrtNFFactor()*theFinalLevel->GetECConversionFactor(chp);
		    externalWidth-=ecNormParam*theInitialChannelGamma*integrals;
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
  return externalWidth;
}

/*!
 * Returns a pointer to the particle pair specified by a position in the PPair vector.
 */

PPair *CNuc::GetPair(int pairNum) {
  PPair *b = &pairs_[pairNum-1];
  return b;
}

/*!
 * Returns a pointer to the \f$ J^\pi \f$ group specified by a position in the JGroup vector.
 */

JGroup *CNuc::GetJGroup(int jGroupNum) {
  JGroup *b = &jgroups_[jGroupNum-1];
  return b;
}

/*!
 * Creates a new copy of the CNuc object in memory and returns a pointer to the new object.
 * Used in AZURECalc function class for thread safety.
 */

CNuc *CNuc::Clone() const {
  CNuc *localCompound = new CNuc(*this);
  return localCompound;
}
