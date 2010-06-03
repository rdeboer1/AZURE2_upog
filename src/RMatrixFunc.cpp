#include "RMatrixFunc.h"
#include "EPoint.h"

/*!
 * The RMatrixFunc object is created with reference to a CNuc object.
 */

RMatrixFunc::RMatrixFunc(CNuc* compound) :
  compound_(compound) {}	

/*!
 * Returns an R-Matrix element specified by positions in the JGroup and AChannel vectors. 
 */

double RMatrixFunc::GetRMatrixElement(int jGroupNum, int channelNum, int channelPrimeNum) const {
  return r_matrices_[jGroupNum-1][channelNum-1][channelPrimeNum-1];
}

/*!
 * Returns an \f$ [1-RL] \f$ Matrix element specified by positions in the JGroup and AChannel vectors. 
 */

std::complex<double> RMatrixFunc::GetRLMatrixElement(int jGroupNum, int channelNum, int channelPrimeNum) const {
  return rl_matrices_[jGroupNum-1][channelNum-1][channelPrimeNum-1];
}

/*!
 * Returns a \f$ [1-RL]^{-1} \f$ Matrix element specified by positions in the JGroup and AChannel vectors. 
 */

std::complex<double> RMatrixFunc::GetRLInvMatrixElement(int jGroupNum, int channelNum, int channelPrimeNum) const {
  return rl_inv_matrices_[jGroupNum-1][channelNum-1][channelPrimeNum-1];
}

/*!
 * Returns a \f$ [1-RL]^{-1}R \f$ Matrix element specified by positions in the JGroup and AChannel vectors. 
 */

std::complex<double> RMatrixFunc::GetRLInvRMatrixElement(int jGroupNum, int channelNum, int channelPrimeNum) const {
  return rl_inv_r_matrices_[jGroupNum-1][channelNum-1][channelPrimeNum-1];
}

/*!
 * Returns an entire \f$ [1-RL] \f$ Matrix specified by a position in the JGroup vector. 
 */

std::vector<std::vector<std::complex<double> > > *RMatrixFunc::GetJSpecRLMatrix(int jGroupNum) {
  std::vector<std::vector<std::complex<double> > > *b=&rl_matrices_[jGroupNum-1];
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
	  double sum=0.0;
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
	      sum+=gammaCh*gammaChp/(resenergy-inenergy);
	    }
	  }
	  this->AddRMatrixElement(j,ch,chp,sum);
	  std::complex<double> loElement =point->GetLoElement(j,chp);
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
      std::vector<std::vector<std::complex<double> > > *theRLMatrix = this->GetJSpecRLMatrix(j);
      std::vector<std::vector<std::complex<double> > > theInvRLMatrix=MatInv(*theRLMatrix);
      this->AddRLInvMatrix(theInvRLMatrix);
      for(int ch=1;ch<=compound()->GetJGroup(j)->NumChannels();ch++) {
	for(int chp=1;chp<=compound()->GetJGroup(j)->NumChannels();chp++) {
	  std::complex<double> rlinvrElement(0.,0.);
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
  int ir=compound()->GetPairNumFromKey(point->GetExitKey());
  Decay *theDecay=compound()->GetPair(aa)->GetDecay(ir);
  for(int k=1;k<=theDecay->NumKGroups();k++) {
    for(int m=1;m<=theDecay->GetKGroup(k)->NumMGroups();m++) {
      MGroup *theMGroup=theDecay->GetKGroup(k)->GetMGroup(m);
      JGroup *theJGroup=compound()->GetJGroup(theMGroup->GetJNum());
      AChannel *entranceChannel=theJGroup->GetChannel(theMGroup->GetChNum());
      AChannel *exitChannel=theJGroup->GetChannel(theMGroup->GetChpNum());
      std::complex<double> uphase=point->GetExpCoulombPhase(theMGroup->GetJNum(),theMGroup->GetChNum())*
	point->GetExpHardSpherePhase(theMGroup->GetJNum(),theMGroup->GetChNum())*
	point->GetExpCoulombPhase(theMGroup->GetJNum(),theMGroup->GetChpNum())*
	point->GetExpHardSpherePhase(theMGroup->GetJNum(),theMGroup->GetChpNum());
      std::complex<double> umatrix=2.0*std::complex<double>(0.0,1.0)*
	point->GetSqrtPenetrability(theMGroup->GetJNum(),theMGroup->GetChNum())*
	point->GetSqrtPenetrability(theMGroup->GetJNum(),theMGroup->GetChpNum())*
	this->GetRLInvRMatrixElement(theMGroup->GetJNum(),
				    theMGroup->GetChpNum(),
				    theMGroup->GetChNum());
      std::complex<double> tphase=point->GetExpCoulombPhase(theMGroup->GetJNum(),theMGroup->GetChNum())*
	point->GetExpCoulombPhase(theMGroup->GetJNum(),theMGroup->GetChNum());
      std::complex<double> tmatrix;
      if(theMGroup->GetChNum()==theMGroup->GetChpNum()) {
	tmatrix=tphase-uphase*(1.0+umatrix);
      } else tmatrix=-uphase*umatrix;
      this->AddTMatrixElement(k,m,tmatrix);
    }
    for(int m=1;m<=theDecay->GetKGroup(k)->NumECMGroups();m++) {
      ECMGroup *theECMGroup=theDecay->GetKGroup(k)->GetECMGroup(m);
      int ecNum=theECMGroup->GetECNum();
      ALevel *finalLevel=compound()->GetJGroup(compound()->GetPair(aa)->GetECLevel(ecNum)->GetJGroupNum())
	->GetLevel(compound()->GetPair(aa)->GetECLevel(ecNum)->GetLevelNum());
      double ecNormParam=finalLevel->GetFitGamma(theECMGroup->GetFinalChannel())*
	finalLevel->GetSqrtNFFactor()*finalLevel->GetECConversionFactor(theECMGroup->GetFinalChannel());
      std::complex<double> tmatrix=ecNormParam*point->GetECAmplitude(k,m);
      if(theECMGroup->IsChannelCapture()) {
	MGroup *chanMGroup=compound()->GetPair(aa)->GetDecay(theECMGroup->GetChanCapDecay())
	  ->GetKGroup(theECMGroup->GetChanCapKGroup())->GetMGroup(theECMGroup->GetChanCapMGroup());
	AChannel *chanEntranceChannel=compound()->GetJGroup(chanMGroup->GetJNum())
	  ->GetChannel(chanMGroup->GetChNum());
	AChannel *chanExitChannel=compound()->GetJGroup(chanMGroup->GetJNum())
	  ->GetChannel(chanMGroup->GetChpNum());
	std::complex<double> umatrix=2.0*std::complex<double>(0.0,1.0)*
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

/*!
 * This function adds an R-Matrix element specified by positions in the JGroup and AChannel vectors.
 */

void RMatrixFunc::AddRMatrixElement(int jGroupNum, int channelNum, int channelPrimeNum, double matrixElement) {
  std::vector<std::vector<double> > e;
  std::vector<double> f;
  while(jGroupNum>r_matrices_.size()) r_matrices_.push_back(e);
  while(channelNum>r_matrices_[jGroupNum-1].size()) r_matrices_[jGroupNum-1].push_back(f);
  r_matrices_[jGroupNum-1][channelNum-1].push_back(matrixElement);
  assert(channelPrimeNum=r_matrices_[jGroupNum-1][channelNum-1].size());
}

/*!
 * This function adds a \f$ [1-RL] \f$ matrix element specified by positions in the JGroup and AChannel vectors.
 */

void RMatrixFunc::AddRLMatrixElement(int jGroupNum, int channelNum, int channelPrimeNum, std::complex<double> matrixElement) {
  std::vector<std::vector<std::complex<double> > > e;
  std::vector<std::complex<double> > f;
  while(jGroupNum>rl_matrices_.size()) rl_matrices_.push_back(e);
  while(channelNum>rl_matrices_[jGroupNum-1].size()) rl_matrices_[jGroupNum-1].push_back(f);
  rl_matrices_[jGroupNum-1][channelNum-1].push_back(matrixElement);
  assert(channelPrimeNum=rl_matrices_[jGroupNum-1][channelNum-1].size());
}

/*!
 * This function adds a \f$ [1-RL]^{-1}R \f$ matrix element specified by positions in the JGroup and AChannel vectors.
 */

void RMatrixFunc::AddRLInvRMatrixElement(int jGroupNum, int channelNum, int channelPrimeNum, std::complex<double> matrixElement) {
  std::vector<std::vector<std::complex<double> > > e;
  std::vector<std::complex<double> > f;
  while(jGroupNum>rl_inv_r_matrices_.size()) rl_inv_r_matrices_.push_back(e);
  while(channelNum>rl_inv_r_matrices_[jGroupNum-1].size()) rl_inv_r_matrices_[jGroupNum-1].push_back(f);
  rl_inv_r_matrices_[jGroupNum-1][channelNum-1].push_back(matrixElement);
  assert(channelPrimeNum=rl_inv_r_matrices_[jGroupNum-1][channelNum-1].size());
}

/*!
 * This function adds an entire \f$ [1-RL]^{-1} \f$ matrix to a vector.
 */

void RMatrixFunc::AddRLInvMatrix(std::vector<std::vector <std::complex<double> > > matrix) {
  rl_inv_matrices_.push_back(matrix);
}
