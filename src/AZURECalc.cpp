#include "AZURECalc.h"
#include "Config.h"
#include "CNuc.h"
#include "EData.h"
#include <iostream>
#include <iomanip>

double AZURECalc::operator()(const vector_r&p) const {

  int thisIteration=data()->Iterations();
  bool isFit=data()->IsFit();
  data()->Iterate();

  CNuc * localCompound = NULL;
  EData *localData = NULL;
  if(isFit) {
    localCompound = compound()->Clone();
    localData = data()->Clone();
  } else {
    localCompound = compound();
    localData = data();
  }

  //Fill Compound Nucleus From Minuit Parameters
  localCompound->FillCompoundFromParams(p);
  
  //loop over segments and points
  double chiSquared=0.0;
  for(int i=1;i<=localData->NumSegments();i++) {
    double segmentChiSquared=0.0;
    for(int ii=1;ii<=localData->GetSegment(i)->NumPoints();ii++) {
      EPoint *point = localData->GetSegment(i)->GetPoint(ii);
      if(!point->IsMapped()) point->Calculate(localCompound,configure());
      double fitCrossSection=point->GetFitCrossSection();
      double CrossSection=point->GetCMCrossSection();
      double CrossSectionError=point->GetCMCrossSectionError();
      double chi=(fitCrossSection-CrossSection)/CrossSectionError;
      double pointChiSquared=pow(chi,2.0);
      segmentChiSquared+=pointChiSquared;
    }
    if(!isFit) data()->GetSegment(i)->SetSegmentChiSquared(segmentChiSquared);
    chiSquared+=segmentChiSquared;
  }

  if(!localData->IsErrorAnalysis()&&thisIteration!=0) {
    if(thisIteration%100==0) std::cout << std::setw(15) << thisIteration
				       << std::setw(15) << chiSquared  
				       << std::endl;
    if(thisIteration%1000==0) {
      localData->WriteOutputFiles(configure());
      localCompound->TransformOut(configure().isEC);
      localCompound->PrintTransformParams(configure().outputdir);
    }
  }
  if(isFit) {
    delete localCompound;
    delete localData;
  }
  return chiSquared;
}
