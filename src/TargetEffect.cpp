#include "TargetEffect.h"
#include <sstream>
#include <iostream>


TargetEffect::TargetEffect(std::ifstream &stream) {
  int isActive;
  std::string segmentList;
  int numIntegrationPoints;
  int isConvolution;
  double sigma;
  int isTargetIntegration;
  double density;
  std::string stoppingPowerEq;
  int numParameters;
  vector_r parameters; 
  stream >> isActive >> segmentList >> numIntegrationPoints >> isConvolution
	 >> sigma >> isTargetIntegration >> density >> stoppingPowerEq 
	 >> numParameters;
  if(!stream.eof()) {
   for(int i=0;i<numParameters;i++) {
      double tempParameter;
      stream >> tempParameter;
      parameters.push_back(tempParameter);
    }
    size_t found=0;
    while(found!=std::string::npos) {
      found=segmentList.find('\"');
      if(found!=std::string::npos) segmentList.erase(found,1);
    }
    found=0;
    while(found!=std::string::npos) {
      found=stoppingPowerEq.find('\"');
      if(found!=std::string::npos) stoppingPowerEq.erase(found,1);
    }
    if(isActive==1) isActive_=true;
    else isActive_=false;
    segmentsList_=segmentList;
    numIntegrationPoints_=numIntegrationPoints;
    if(isConvolution==1) isConvolution_=true;
    else isConvolution_=false;
    sigma_=sigma;
    if(isTargetIntegration==1) isTargetIntegration_=true;
    else isTargetIntegration_=false;
    density_=density;
    if(isTargetIntegration_) {
      stoppingPowerEq_.Initialize(stoppingPowerEq,numParameters);
      for(int i=0;i<numParameters;i++) {
	stoppingPowerEq_.SetParameter(i,parameters[i]);
      }
    }
  }
}

bool TargetEffect::IsActive() const {
  return isActive_;
}

bool TargetEffect::IsConvolution() const {
  return isConvolution_;
}

bool TargetEffect::IsTargetIntegration() const {
  return isTargetIntegration_;
}

int TargetEffect::NumSubPoints() const {
  return numIntegrationPoints_;
}

double TargetEffect::GetSigma() const {
  return sigma_;
}

double TargetEffect::GetDensity() const {
  return density_;
}

double TargetEffect::TargetThickness(double energy)  {
  return this->GetStoppingPowerEq()->Evaluate(energy)*this->GetDensity();
}

void TargetEffect::SetNumSubPoints(int numPoints) {
  numIntegrationPoints_=numPoints;
}

std::vector<int> TargetEffect::GetSegmentsList() const {
  std::vector<int> tempList;
  int i=0;
  int lastSegNum=0;
  bool inclusive=false;
  while(i<segmentsList_.length()) {
    if(segmentsList_[i]>='0'&&segmentsList_[i]<='9') {
      std::string tempString;
      while(segmentsList_[i]!=','&&segmentsList_[i]!='-'&&
	    i<segmentsList_.length()) {
	tempString+=segmentsList_[i];
	i++;
      }
      std::istringstream stm;
      stm.str(tempString);
      int tempSegNum;stm>>tempSegNum;
      if(inclusive==true) for(int j=lastSegNum+1;j<=tempSegNum;j++) 
			    tempList.push_back(j);
      else tempList.push_back(tempSegNum);
      lastSegNum=tempSegNum;
    }
    if(segmentsList_[i]=='-') inclusive=true;
    else inclusive =false;
    i++;
  }      
  return tempList;
}

Equation *TargetEffect::GetStoppingPowerEq() {
  Equation *tempEquation;
  tempEquation=&stoppingPowerEq_;
  return tempEquation;
}

double TargetEffect::GetConvolutionFactor(double energy, double centroid) const {
  double sigma=this->GetSigma();
  return pow(2.*pi,-0.5)/sigma*exp(-pow(energy-centroid,2.0)/2.0/pow(sigma,2.0));
}
