#include "AZURECalc.h"
#include "Config.h"
#include "CNuc.h"
#include "EData.h"
#include <iostream>
#include <iomanip>

double AZURECalc::operator()(const vector_r& p) const {

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
  localData->FillNormsFromParams(p);
  if(configure().isBrune) localCompound->CalcShiftFunctions();
  
  //loop over segments and points
  double chiSquared=0.0;
  double segmentChiSquared=0.0;
  for(EDataIterator data=localData->begin();data!=localData->end();data++) {
    if(data.segment()->GetPoints().begin()==data.point()) segmentChiSquared=0.0;
    if(!data.point()->IsMapped()) data.point()->Calculate(localCompound,configure());
    double fitCrossSection=data.point()->GetFitCrossSection();
    double dataNorm=data.segment()->GetNorm();
    double CrossSection=data.point()->GetCMCrossSection()*dataNorm;
    double CrossSectionError=data.point()->GetCMCrossSectionError()*dataNorm;
    double chi=(fitCrossSection-CrossSection)/CrossSectionError;
    double pointChiSquared=pow(chi,2.0);
    segmentChiSquared+=pointChiSquared;
    if(data.segment()->GetPoints().end()-1==data.point()) {
      if(!isFit) data.segment()->SetSegmentChiSquared(segmentChiSquared);
      chiSquared+=segmentChiSquared;
    }
  }

  if(!localData->IsErrorAnalysis()&&thisIteration!=0) {
    if(thisIteration%100==0) std::cout << "\r\tIteration: " << std::setw(6) << thisIteration
				       << " Chi-Squared: " << chiSquared;  std::cout.flush();

    if(thisIteration%1000==0) {
      localData->WriteOutputFiles(configure());
      localCompound->TransformOut(configure());
      localCompound->PrintTransformParams(configure().outputdir);
    }
  }
  if(isFit) {
    delete localCompound;
    delete localData;
  }
  return chiSquared;
}
