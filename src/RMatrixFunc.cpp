#include "EPoint.h"
#include "CNuc.h"
#include "MatInv.h"
#include "RMatrixFunc.h"
#include <assert.h>

/*!
 * The RMatrixFunc object is created with reference to a CNuc object.
 */

RMatrixFunc::RMatrixFunc(CNuc* compound, const struct Config &configure) :
  compound_(compound), configure_(configure) {}	

/*!
 * Returns an R-Matrix element specified by positions in the JGroup and AChannel vectors. 
 */

complex RMatrixFunc::GetRMatrixElement(int jGroupNum, int channelNum, int channelPrimeNum) const {
  return r_matrices_[jGroupNum-1][channelNum-1][channelPrimeNum-1];
}

/*!
 * Returns an \f$ [1-RL] \f$ Matrix element specified by positions in the JGroup and AChannel vectors. 
 */

complex RMatrixFunc::GetRLMatrixElement(int jGroupNum, int channelNum, int channelPrimeNum) const {
  return rl_matrices_[jGroupNum-1][channelNum-1][channelPrimeNum-1];
}

/*!
 * Returns a \f$ [1-RL]^{-1} \f$ Matrix element specified by positions in the JGroup and AChannel vectors. 
 */

complex RMatrixFunc::GetRLInvMatrixElement(int jGroupNum, int channelNum, int channelPrimeNum) const {
  return rl_inv_matrices_[jGroupNum-1][channelNum-1][channelPrimeNum-1];
}

/*!
 * Returns a \f$ [1-RL]^{-1}R \f$ Matrix element specified by positions in the JGroup and AChannel vectors. 
 */

complex RMatrixFunc::GetRLInvRMatrixElement(int jGroupNum, int channelNum, int channelPrimeNum) const {
  return rl_inv_r_matrices_[jGroupNum-1][channelNum-1][channelPrimeNum-1];
}

/*!
 * Returns an entire \f$ [1-RL] \f$ Matrix specified by a position in the JGroup vector. 
 */

matrix_c *RMatrixFunc::GetJSpecRLMatrix(int jGroupNum) {
  matrix_c *b=&rl_matrices_[jGroupNum-1];
  return b;
}

/*!
 * Clears all matrices associated with the RMatrixFunc object.
 */

void RMatrixFunc::ClearMatrices() {
  r_matrices_.clear();
  rl_matrices_.clear();
  rl_inv_matrices_.clear();
  rl_inv_r_matrices_.clear();
  tmatrix_.clear();
  ec_tmatrix_.clear();
}

/*!
 * This function creates the \f$ [1-RL] \f$ and \f$ R \f$ Matrices from the CNuc object.
 */

void RMatrixFunc::FillMatrices (EPoint *point) {
  for(int j=1;j<=compound()->NumJGroups();j++) {
    if(compound()->GetJGroup(j)->IsInRMatrix()) {
      for(int ch=1;ch<=compound()->GetJGroup(j)->NumChannels();ch++) {
	for(int chp=1;chp<=compound()->GetJGroup(j)->NumChannels();chp++) {
	  complex sum(0.0,0.0);
	  for(int la=1;la<=compound()->GetJGroup(j)->NumLevels();la++) {
	    if(compound()->GetJGroup(j)->GetLevel(la)->IsInRMatrix()) {
	      ALevel *level=compound()->GetJGroup(j)->GetLevel(la);
	      double gammaCh=level->GetFitGamma(ch);
	      double gammaChp=level->GetFitGamma(chp);
	      double resenergy=level->GetFitE();
	      double inenergy=point->GetCMEnergy()+
		compound()->
		GetPair(compound()->GetPairNumFromKey(point->GetEntranceKey()))->
		GetSepE()+
		compound()->
		GetPair(compound()->GetPairNumFromKey(point->GetEntranceKey()))->
		GetExE();
	      double gammaSum=0.;
	      if(configure().useRMC) 
		for(int chpp=1;chpp<=compound()->GetJGroup(j)->NumChannels();chpp++) 
		  if(compound()->GetJGroup(j)->GetChannel(chpp)->GetRadType()!='P')
		    gammaSum+=pow(compound()->GetJGroup(j)->GetLevel(la)->GetFitGamma(chpp),2.0);
	      sum+=gammaCh*gammaChp/(resenergy-inenergy-complex(0.0,1.0)*gammaSum);
	    }
	  }
	  this->AddRMatrixElement(j,ch,chp,sum);
	  complex loElement =point->GetLoElement(j,chp);
	  if(ch==chp) this->AddRLMatrixElement(j,ch,chp,1.0-sum*loElement);
	  else this->AddRLMatrixElement(j,ch,chp,-sum*loElement);
	}
      }
    }
  }
}


/*!
 * This function inverts the \f$ [1-RL] \f$ matrices and creates the \f$ [1-RL]^{-1}R \f$ matrices.
 */

void RMatrixFunc::InvertMatrices() {
  for(int j=1;j<=compound()->NumJGroups();j++) {
    if(compound()->GetJGroup(j)->IsInRMatrix()) {
      matrix_c *theRLMatrix = this->GetJSpecRLMatrix(j);
      matrix_c theInvRLMatrix=MatInv(*theRLMatrix);
      this->AddRLInvMatrix(theInvRLMatrix);
      for(int ch=1;ch<=compound()->GetJGroup(j)->NumChannels();ch++) {
	for(int chp=1;chp<=compound()->GetJGroup(j)->NumChannels();chp++) {
	  complex rlinvrElement(0.,0.);
	  for(int chpp=1;chpp<=compound()->GetJGroup(j)->NumChannels();chpp++) {
	    rlinvrElement+=this->GetRLInvMatrixElement(j,ch,chpp)*
	      this->GetRMatrixElement(j,chpp,chp);
	  }
	  this->AddRLInvRMatrixElement(j,ch,chp,rlinvrElement);
	}
      }
    }
  }
}

/*!
 * This function calculates the T-Matrix for each reaction pathways based on the \f$ [1-RL]^{-1}R \f$ matrix.
 */

void RMatrixFunc::CalculateTMatrix(EPoint *point) {
  int aa=compound()->GetPairNumFromKey(point->GetEntranceKey());
  int irEnd;
  int irStart;
  bool isRMC=false;
  if(configure().useRMC && 
     compound()->GetPair(compound()->GetPairNumFromKey(point->GetExitKey()))->GetPType()==10) {
    irStart=1;
    irEnd=compound()->GetPair(aa)->NumDecays();
    isRMC=true;
  } else {
    irStart=0;
    while(irStart<compound()->GetPair(aa)->NumDecays()) {
      irStart++;
      if(compound()->GetPair(aa)->GetDecay(irStart)->GetPairNum()==
	 compound()->GetPairNumFromKey(point->GetExitKey())) break;
    }
    irEnd=irStart;
  }
  for(int ir=irStart;ir<=irEnd;ir++) {
    Decay *theDecay=compound()->GetPair(aa)->GetDecay(ir);
    for(int k=1;k<=theDecay->NumKGroups();k++) {
      for(int m=1;m<=theDecay->GetKGroup(k)->NumMGroups();m++) {
	MGroup *theMGroup=theDecay->GetKGroup(k)->GetMGroup(m);
	JGroup *theJGroup=compound()->GetJGroup(theMGroup->GetJNum());
	AChannel *entranceChannel=theJGroup->GetChannel(theMGroup->GetChNum());
	AChannel *exitChannel=theJGroup->GetChannel(theMGroup->GetChpNum());
	complex uphase=point->GetExpCoulombPhase(theMGroup->GetJNum(),theMGroup->GetChNum())*
	  point->GetExpHardSpherePhase(theMGroup->GetJNum(),theMGroup->GetChNum())*
	  point->GetExpCoulombPhase(theMGroup->GetJNum(),theMGroup->GetChpNum())*
	  point->GetExpHardSpherePhase(theMGroup->GetJNum(),theMGroup->GetChpNum());
	complex umatrix=2.0*std::complex<double>(0.0,1.0)*
	  point->GetSqrtPenetrability(theMGroup->GetJNum(),theMGroup->GetChNum())*
	  point->GetSqrtPenetrability(theMGroup->GetJNum(),theMGroup->GetChpNum())*
	  this->GetRLInvRMatrixElement(theMGroup->GetJNum(),
				       theMGroup->GetChpNum(),
				       theMGroup->GetChNum());
	complex tphase=point->GetExpCoulombPhase(theMGroup->GetJNum(),theMGroup->GetChNum())*
	  point->GetExpCoulombPhase(theMGroup->GetJNum(),theMGroup->GetChNum());
	complex tmatrix;
	if(isRMC) this->AddTMatrixElement(k,m,complex(0.0,-1.0)*umatrix,ir);
	else {
	  if(theMGroup->GetChNum()==theMGroup->GetChpNum()) {
	    tmatrix=tphase-uphase*(1.0+umatrix);
	  } else tmatrix=-uphase*umatrix;
	  this->AddTMatrixElement(k,m,tmatrix);
	}
      }
      for(int m=1;m<=theDecay->GetKGroup(k)->NumECMGroups();m++) {
	ECMGroup *theECMGroup=theDecay->GetKGroup(k)->GetECMGroup(m);
	ALevel *finalLevel=compound()->GetJGroup(theECMGroup->GetJGroupNum())
	  ->GetLevel(theECMGroup->GetLevelNum());
	double ecNormParam=finalLevel->GetFitGamma(theECMGroup->GetFinalChannel())*
	  finalLevel->GetSqrtNFFactor()*finalLevel->GetECConversionFactor(theECMGroup->GetFinalChannel());
	complex tmatrix=ecNormParam*point->GetECAmplitude(k,m);
	if(theECMGroup->IsChannelCapture()) {
	  MGroup *chanMGroup=compound()->GetPair(aa)->GetDecay(theECMGroup->GetChanCapDecay())
	    ->GetKGroup(theECMGroup->GetChanCapKGroup())->GetMGroup(theECMGroup->GetChanCapMGroup());
	  AChannel *chanEntranceChannel=compound()->GetJGroup(chanMGroup->GetJNum())
	    ->GetChannel(chanMGroup->GetChNum());
	  AChannel *chanExitChannel=compound()->GetJGroup(chanMGroup->GetJNum())
	    ->GetChannel(chanMGroup->GetChpNum());
	  complex umatrix=2.0*std::complex<double>(0.0,1.0)*
	    point->GetSqrtPenetrability(chanMGroup->GetJNum(),chanMGroup->GetChNum())*
	    this->GetRLInvRMatrixElement(chanMGroup->GetJNum(),
					 chanMGroup->GetChpNum(),
					 chanMGroup->GetChNum());
	  tmatrix=tmatrix*umatrix;
	} 
	this->AddECTMatrixElement(k,m,tmatrix);
      }
    }
  }
}

/*!
 * This function adds an R-Matrix element specified by positions in the JGroup and AChannel vectors.
 */

void RMatrixFunc::AddRMatrixElement(int jGroupNum, int channelNum, int channelPrimeNum, complex matrixElement) {
  matrix_c e;
  vector_c f;
  while(jGroupNum>r_matrices_.size()) r_matrices_.push_back(e);
  while(channelNum>r_matrices_[jGroupNum-1].size()) r_matrices_[jGroupNum-1].push_back(f);
  r_matrices_[jGroupNum-1][channelNum-1].push_back(matrixElement);
  assert(channelPrimeNum=r_matrices_[jGroupNum-1][channelNum-1].size());
}

/*!
 * This function adds a \f$ [1-RL] \f$ matrix element specified by positions in the JGroup and AChannel vectors.
 */

void RMatrixFunc::AddRLMatrixElement(int jGroupNum, int channelNum, int channelPrimeNum, complex matrixElement) {
  matrix_c e;
  vector_c f;
  while(jGroupNum>rl_matrices_.size()) rl_matrices_.push_back(e);
  while(channelNum>rl_matrices_[jGroupNum-1].size()) rl_matrices_[jGroupNum-1].push_back(f);
  rl_matrices_[jGroupNum-1][channelNum-1].push_back(matrixElement);
  assert(channelPrimeNum=rl_matrices_[jGroupNum-1][channelNum-1].size());
}

/*!
 * This function adds a \f$ [1-RL]^{-1}R \f$ matrix element specified by positions in the JGroup and AChannel vectors.
 */

void RMatrixFunc::AddRLInvRMatrixElement(int jGroupNum, int channelNum, int channelPrimeNum, complex matrixElement) {
  matrix_c e;
  vector_c f;
  while(jGroupNum>rl_inv_r_matrices_.size()) rl_inv_r_matrices_.push_back(e);
  while(channelNum>rl_inv_r_matrices_[jGroupNum-1].size()) rl_inv_r_matrices_[jGroupNum-1].push_back(f);
  rl_inv_r_matrices_[jGroupNum-1][channelNum-1].push_back(matrixElement);
  assert(channelPrimeNum=rl_inv_r_matrices_[jGroupNum-1][channelNum-1].size());
}

/*!
 * This function adds an entire \f$ [1-RL]^{-1} \f$ matrix to a vector.
 */

void RMatrixFunc::AddRLInvMatrix(std::vector<std::vector <complex > > matrix) {
  rl_inv_matrices_.push_back(matrix);
}
