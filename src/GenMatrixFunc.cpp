#include "CNuc.h"
#include "EPoint.h"
#include "GenMatrixFunc.h"
#include <assert.h>

/*!
 * The child classes AMatrixFunc or RMatrixFunc contain functions
 * to calculate the T-Matrix from the fitted R-Matrix parameters.  This function
 * then calculates the cross section from the T-Matrix elements.
 */

void GenMatrixFunc::CalculateCrossSection(EPoint *point) {
  complex sum(0.,0.);
  int aa=compound()->GetPairNumFromKey(point->GetEntranceKey());
  int ir=0;
  while(ir<compound()->GetPair(aa)->NumDecays()) {
    ir++;
    if(compound()->GetPair(aa)->GetDecay(ir)->GetPairNum()==compound()->GetPairNumFromKey(point->GetExitKey())) break;
  }
  Decay *theDecay=compound()->GetPair(aa)->GetDecay(ir);
  if(compound()->GetPair(compound()->GetPairNumFromKey(point->GetExitKey()))->GetPType()==10 &&
     (configure().paramMask & Config::USE_RMC_FORMALISM)) {
    int decayNum=0;
    while(decayNum<compound()->GetPair(aa)->NumDecays()) {
      decayNum++;
      if(compound()->GetPair(aa)->GetDecay(decayNum)->GetPairNum()==aa) break;
    }
    for(int k=1;k<=compound()->GetPair(aa)->GetDecay(decayNum)->NumKGroups();k++) {
      for(int m=1;m<=compound()->GetPair(aa)->GetDecay(decayNum)->GetKGroup(k)->NumMGroups();m++) {
	MGroup *theMGroup=compound()->GetPair(aa)->GetDecay(decayNum)->GetKGroup(k)->GetMGroup(m);
	if(theMGroup->GetChNum()==theMGroup->GetChpNum()) {
	  double jValue=compound()->GetJGroup(theMGroup->GetJNum())->GetJ();
	  sum+=2.*point->GetGeometricalFactor()*
	    (2.*jValue+1.)*compound()->GetPair(aa)->GetI1I2Factor()*    
	    imag(this->GetTMatrixElement(k,m,decayNum));
	}
      }
    }
    for(int dp=1;dp<=compound()->GetPair(aa)->NumDecays();dp++) {
      if(compound()->GetPair(compound()->GetPair(aa)->GetDecay(dp)->GetPairNum())->GetPType()==0) {
	for(int k=1;k<=compound()->GetPair(aa)->GetDecay(dp)->NumKGroups();k++) {
	  this->ClearTempTMatrices();
	  for(int m=1;m<=compound()->GetPair(aa)->GetDecay(dp)->GetKGroup(k)->NumMGroups();m++) {
	    MGroup *theMGroup=compound()->GetPair(aa)->GetDecay(dp)->GetKGroup(k)->GetMGroup(m);
	    int lValue=compound()->GetJGroup(theMGroup->GetJNum())->GetChannel(theMGroup->GetChNum())->GetL();
	    int lpValue=compound()->GetJGroup(theMGroup->GetJNum())->GetChannel(theMGroup->GetChpNum())->GetL();
	    double jValue=compound()->GetJGroup(theMGroup->GetJNum())->GetJ();
	    int tempTNum=this->IsTempTMatrix(jValue,lValue,lpValue);
	    if(!tempTNum) {
	      TempTMatrix temptmatrix={jValue,lValue,lpValue,this->GetTMatrixElement(k,m,dp)};
	      this->NewTempTMatrix(temptmatrix);
	    } else this->AddToTempTMatrix(tempTNum,this->GetTMatrixElement(k,m,dp));
	  }
	  for(int temp=1;temp<=this->NumTempTMatrices();temp++) {
	    sum-=point->GetGeometricalFactor()*
	      (2.*this->GetTempTMatrix(temp)->jValue+1.)*
	      compound()->GetPair(aa)->GetI1I2Factor()*
	      (this->GetTempTMatrix(temp)->TMatrix)*conj(this->GetTempTMatrix(temp)->TMatrix);
	  }
	}
      }
    }
    point->SetFitCrossSection(real(sum)/100.);
  } else {
    if(!point->IsPhase()) {
      double angleIntegratedXS=0.;
      if(!point->IsDifferential()) {
	for(int k=1;k<=theDecay->NumKGroups();k++) {
	  this->ClearTempTMatrices();
	  for(int m=1;m<=theDecay->GetKGroup(k)->NumMGroups();m++) {
	    if(compound()->GetPair(aa)->GetPType()==20) {
	      sum+=25.*this->GetTMatrixElement(k,m)*conj(this->GetTMatrixElement(k,m));
	    } else { 
	      MGroup *theMGroup=theDecay->GetKGroup(k)->GetMGroup(m);
	      int lValue=compound()->GetJGroup(theMGroup->GetJNum())->GetChannel(theMGroup->GetChNum())->GetL();
	      int lpValue=compound()->GetJGroup(theMGroup->GetJNum())->GetChannel(theMGroup->GetChpNum())->GetL();
	      double jValue=compound()->GetJGroup(theMGroup->GetJNum())->GetJ();
	      int tempTNum=this->IsTempTMatrix(jValue,lValue,lpValue);
	      if(!tempTNum) {
		TempTMatrix temptmatrix={jValue,lValue,lpValue,this->GetTMatrixElement(k,m)};
		this->NewTempTMatrix(temptmatrix);
	      } else this->AddToTempTMatrix(tempTNum,this->GetTMatrixElement(k,m));
	    }
	  }
	  if(compound()->GetPair(aa)->GetPType()==20) continue;
	  for(int m=1;m<=theDecay->GetKGroup(k)->NumECMGroups();m++) {
	    ECMGroup *theECMGroup=theDecay->GetKGroup(k)->GetECMGroup(m);
	    int lValue=theECMGroup->GetL();
	    int lpValue=theECMGroup->GetMult();
	    double jValue=theECMGroup->GetJ();
	    int tempTNum=this->IsTempTMatrix(jValue,lValue,lpValue);
	    if(!tempTNum) {
	      TempTMatrix temptmatrix={jValue,lValue,lpValue,this->GetECTMatrixElement(k,m)};
	      this->NewTempTMatrix(temptmatrix);
	    } else this->AddToTempTMatrix(tempTNum,this->GetECTMatrixElement(k,m));
	  }
	  for(int temp=1;temp<=this->NumTempTMatrices();temp++) {
	    sum+=point->GetGeometricalFactor()*
	      (2.*this->GetTempTMatrix(temp)->jValue+1.)*
	      compound()->GetPair(aa)->GetI1I2Factor()*
	      (this->GetTempTMatrix(temp)->TMatrix)*conj(this->GetTempTMatrix(temp)->TMatrix);
	  }
	}
	angleIntegratedXS=real(sum)/100.;
	if(!point->IsAngularDist()) {
	  point->SetFitCrossSection(angleIntegratedXS);
	  return;
	}
      }   
      std::vector<double> angularCoeff(std::min(point->GetMaxLOrder()+1,point->GetMaxAngDistOrder()+1),0.);
      for(int kL=1;kL<=theDecay->NumKLGroups();kL++) {
	for(int inter=1;inter<=theDecay->GetKLGroup(kL)
	      ->NumInterferences();inter++) {
	  Interference *theInterference=theDecay->GetKLGroup(kL)
	    ->GetInterference(inter);
	  complex T1(0.0,0.0),T2(0.0,0.0);
	  std::string interferenceType=theInterference->GetInterferenceType();
	  if(interferenceType=="RR") {
	    T1=this->GetTMatrixElement(theDecay->GetKLGroup(kL)->GetK(),theInterference->GetM1());
	    T2=this->GetTMatrixElement(theDecay->GetKLGroup(kL)->GetK(),theInterference->GetM2());
	  } else if(interferenceType=="ER") {
	    T1=this->GetECTMatrixElement(theDecay->GetKLGroup(kL)->GetK(),theInterference->GetM1());
	    T2=this->GetTMatrixElement(theDecay->GetKLGroup(kL)->GetK(),theInterference->GetM2());
	  } else if(interferenceType=="RE") {
	    T1=this->GetTMatrixElement(theDecay->GetKLGroup(kL)->GetK(),theInterference->GetM1());
	    T2=this->GetECTMatrixElement(theDecay->GetKLGroup(kL)->GetK(),theInterference->GetM2());
	  } else if(interferenceType=="EE") {
	    T1=this->GetECTMatrixElement(theDecay->GetKLGroup(kL)->GetK(),theInterference->GetM1());
	    T2=this->GetECTMatrixElement(theDecay->GetKLGroup(kL)->GetK(),theInterference->GetM2());
	  }
	  int lOrder = theDecay->GetKLGroup(kL)->GetLOrder();
	  sum+=theInterference->GetZ1Z2()*T1*conj(T2)*
	    point->GetLegendreP(lOrder);
	  if((lOrder < angularCoeff.size()) && point->IsAngularDist()) {
	    double tempCoeff=angularCoeff[lOrder]+
	      real(theInterference->GetZ1Z2()*T1*conj(T2))*point->GetGeometricalFactor()*
	      compound()->GetPair(aa)->GetI1I2Factor()/100.*4./angleIntegratedXS;
	    angularCoeff[lOrder]=tempCoeff;
	  }
	}
      }
      if(point->IsAngularDist()) {
	point->SetAngularDists(angularCoeff);
	return;
      }
      complex RT=sum/pi*point->GetGeometricalFactor()*
	compound()->GetPair(aa)->GetI1I2Factor();
      
      complex CT(0.,0.), IT(0.,0.);
      if(aa==ir) {
	complex coulombAmplitude=point->GetCoulombAmplitude();
	CT=coulombAmplitude*conj(coulombAmplitude)*point->GetGeometricalFactor();
	
	sum=complex(0.,0.);
	for(int k=1;k<=theDecay->NumKGroups();k++) {
	  for(int m=1;m<=theDecay->GetKGroup(k)->NumMGroups();m++) {
	    MGroup *theMGroup=theDecay->GetKGroup(k)->GetMGroup(m);	
	    AChannel *entranceChannel=compound()->GetJGroup(theMGroup->GetJNum())->GetChannel(theMGroup->GetChNum());
	    AChannel *exitChannel=compound()->GetJGroup(theMGroup->GetJNum())->GetChannel(theMGroup->GetChpNum());
	    if(entranceChannel==exitChannel) 
	      sum+=theMGroup->GetStatSpinFactor()*
		coulombAmplitude*conj(this->GetTMatrixElement(k,m))*
		point->GetLegendreP(compound()->GetJGroup(theMGroup->GetJNum())->
				    GetChannel(theMGroup->GetChNum())->GetL());
	  }
	}
	IT=complex(0.,1.)/sqrt(pi)*sum*point->GetGeometricalFactor();
      }
      point->SetFitCrossSection((real(CT)+real(RT)+real(IT))/100.);
    } else if(aa==ir) {
      double segmentJ=point->GetJ();
      int segmentL=point->GetL();
      this->ClearTempTMatrices();
      for(int k=1;k<=theDecay->NumKGroups();k++) {
	for(int m=1;m<=theDecay->GetKGroup(k)->NumMGroups();m++) {
	  MGroup *theMGroup=theDecay->GetKGroup(k)->GetMGroup(m);
	  double jValue=compound()->GetJGroup(theMGroup->GetJNum())->GetJ();
	  AChannel *entranceChannel=compound()->GetJGroup(theMGroup->GetJNum())->GetChannel(theMGroup->GetChNum());
	  int lValue=entranceChannel->GetL();	
	  AChannel *exitChannel=compound()->GetJGroup(theMGroup->GetJNum())->GetChannel(theMGroup->GetChpNum());
	  if(jValue==segmentJ&&lValue==segmentL&&entranceChannel==exitChannel) {
	    complex expCoulPhaseSquared=point->GetExpCoulombPhase(theMGroup->GetJNum(),theMGroup->GetChNum())*
	      point->GetExpCoulombPhase(theMGroup->GetJNum(),theMGroup->GetChNum());
	    complex theUMatrix=(expCoulPhaseSquared-this->GetTMatrixElement(k,m))/expCoulPhaseSquared;
	    int tempTNum=this->IsTempTMatrix(jValue,lValue,lValue);
	    if(!tempTNum) {
	      TempTMatrix temptmatrix={jValue,lValue,lValue,theUMatrix};
	      this->NewTempTMatrix(temptmatrix);
	    } else this->AddToTempTMatrix(tempTNum,theUMatrix);
	  }
	}
      }
      assert(this->NumTempTMatrices()<=1);
      double phase=0.0;
      if(this->NumTempTMatrices()==1) phase = 180.0/pi/2.0*
					atan2(imag(this->GetTempTMatrix(1)->TMatrix),real(this->GetTempTMatrix(1)->TMatrix)); 
      //if(segmentL%2!=0&&phase<0) phase+=180.0;
      point->SetFitCrossSection(phase);
    }
  }
}


/*!
 * Creates a new temporary T-Matrix element.
 */

void GenMatrixFunc::NewTempTMatrix(TempTMatrix tempTMatrix) {
  temp_t_matrices_.push_back(tempTMatrix);
}

/*!
 * Adds a value to the temporary T-Matrix element specified by its position in the TempTMatrix vector.
 */

void GenMatrixFunc::AddToTempTMatrix(int tempTMatrixNum, complex tempValue) {
  this->GetTempTMatrix(tempTMatrixNum)->TMatrix+=tempValue;
  
}

/*!
 * Clears the temporary T-Matrices.
 */

void GenMatrixFunc::ClearTempTMatrices() {
  temp_t_matrices_.clear();
}

/*!
 * Adds an internal T-Matrix element to the vector of internal T-matrix elements 
 * corresponding to a specified internal reaction pathway.
 */

 void GenMatrixFunc::AddTMatrixElement(int kGroupNum ,int mGroupNum,complex tMatrixElement, int decayNum) {
  matrix_c d;
  vector_c e;
  while(decayNum>tmatrix_.size()) tmatrix_.push_back(d);
  while(kGroupNum>tmatrix_[decayNum-1].size()) tmatrix_[decayNum-1].push_back(e);
  tmatrix_[decayNum-1][kGroupNum-1].push_back(tMatrixElement);
  assert(kGroupNum==tmatrix_[decayNum-1].size());
  assert(mGroupNum==tmatrix_[decayNum-1][kGroupNum-1].size());
}

/*!
 * Adds an external T-Matrix element to the vector of external T-matrix elements 
 * corresponding to a specified external reaction pathway.
 */

void GenMatrixFunc::AddECTMatrixElement(int kGroupNum ,int mGroupNum,complex tMatrixElement) {
  vector_c d;
  while(kGroupNum>ec_tmatrix_.size()) ec_tmatrix_.push_back(d);
  ec_tmatrix_[kGroupNum-1].push_back(tMatrixElement);
  assert(mGroupNum==ec_tmatrix_[kGroupNum-1].size());
}

/*!
 * Tests if a temporary T-Matrix element already exists for a given \f$ J,l,l' \f$ combination.
 * If the element exists, returns the position in the TempTMatrix vector, otherwise returns 0.
 */

int GenMatrixFunc::IsTempTMatrix(double jValue, int lValue, int lPrimeValue) {
  int d=0;
  bool e=false;
  while(!e&&d<this->NumTempTMatrices()) {
    if(jValue==this->GetTempTMatrix(d+1)->jValue&&
       lValue==this->GetTempTMatrix(d+1)->lValue&&
       lPrimeValue==this->GetTempTMatrix(d+1)->lpValue) e=true;
    d++;
  }
  if(!e) return 0;
  else return d;
}

/*!
 * Returns the number of temporary T-Matrix elements in the TempTMatrix vector.
 */

int GenMatrixFunc::NumTempTMatrices() const {
  return temp_t_matrices_.size();
}

/*!
 * Returns a pointer to the temporary T-Matrix element specified by a position in the TempTMatrix vector.
 */

TempTMatrix *GenMatrixFunc::GetTempTMatrix(int tempTMatrixNum) {
  TempTMatrix *b=&temp_t_matrices_[tempTMatrixNum-1];
  return b;
}

/*!
 * Returns the value of the internal T-Matrix element specified by an internal reaction pathway.
 */

 complex GenMatrixFunc::GetTMatrixElement(int kGroupNum, int mGroupNum, int decayNum) const {
  return tmatrix_[decayNum-1][kGroupNum-1][mGroupNum-1];
}

/*!
 * Returns the value of the external T-Matrix element specified by an external reaction pathway.
 */

complex GenMatrixFunc::GetECTMatrixElement(int kGroupNum, int ecMGroupNum) const {
  return ec_tmatrix_[kGroupNum-1][ecMGroupNum-1];
}
