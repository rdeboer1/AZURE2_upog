#ifndef TARGETEFFECT_H
#define TARGETEFFECT_H

#include <string>
#include <fstream>			       
#include <vector>
#include "Constants.h"
#include "Equation.h"

///An AZURE target effect entry

/*!
 * Experimential effects including gaussian beam convolution, target 
 * integration, and a combination of the two are grouped under the TargetEffect
 * class. An object is created corresponding to each corresponding entry in 
 * AZURESetup2.
 */

class TargetEffect {
 public:
  TargetEffect(std::ifstream &);
  bool IsActive() const;
  bool IsConvolution() const;
  bool IsTargetIntegration() const;
  int NumSubPoints() const;
  double GetSigma() const;
  double GetDensity() const;
  double TargetThickness(double);
  double GetConvolutionFactor(double, double) const;
  void SetNumSubPoints(int);
  std::vector<int> GetSegmentsList() const;
  Equation *GetStoppingPowerEq();
  static const double convolutionRange=3.;
 private:
  bool isConvolution_;
  bool isTargetIntegration_;
  bool isActive_;
  int numIntegrationPoints_;
  double sigma_;
  double density_;
  Equation stoppingPowerEq_;
  std::string segmentsList_;
};

#endif
