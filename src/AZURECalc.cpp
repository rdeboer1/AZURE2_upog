#include "AZURECalc.h"

double AZURECalc::operator()(const std::vector<double>&p) const {

  data()->iterate();
  
  //Fill Compound Nucleus From Minuit Parameters
  compound()->FillCompoundFromParams(p);
  
  //loop over segments and points
  double chiSquared=0.0;
  for(int i=1;i<=data()->NumSegments();i++) {
    double segmentChiSquared=0.0;
    for(int ii=1;ii<=data()->GetSegment(i)->NumPoints();ii++) {
      EPoint *point=data()->GetSegment(i)->GetPoint(ii);
      if(!point->IsMapped()) point->Calculate(compound(),configure());
      double fitCrossSection=point->GetFitCrossSection();
      double CrossSection=point->GetCMCrossSection();
      double CrossSectionError=point->GetCMCrossSectionError();
      double chi=(fitCrossSection-CrossSection)/CrossSectionError;
      double pointChiSquared=pow(chi,2.0);
      segmentChiSquared+=pointChiSquared;
    }
    data()->GetSegment(i)->SetSegmentChiSquared(segmentChiSquared);
    chiSquared+=segmentChiSquared;
  }

  if(data()->iterations()%100==0) std::cout << std::setw(15) << data()->iterations()
					    << std::setw(15) << chiSquared
					    << std::endl;
  if(data()->iterations()%1000==0) {
    data()->WriteOutputFiles(configure());
    compound()->TransformOut(configure().isEC);
    compound()->PrintTransformParams(configure().outputdir);
  }
  return chiSquared;
}
