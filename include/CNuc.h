#ifndef CNUC_H
#define CNUC_H

#include <string>
#include <iostream>
#include <iomanip>
#include "NFIntegral.h"
#include "JGroup.h"
#include "CoulFunc.h"
#include "ShftFunc.h"
#include "AngCoeff.h"
#include "Minuit2/MnUserParameters.h"
#include "Diagonalize.h"
#include "ECIntegral.h"
#include "Config.h"


///An AZURE compound nucleus

/*!
 * The compound nucleus is the fundamental concept of R-Matrix theory.  As such, the CNuc object
 * in AZURE is the top level container object for all structure and reaction objects.  Specifically,
 * the CNuc object is the container object for vectors of PPair and JGroup objects, within which all other 
 * nuclear data objects are contained.
 */

class CNuc {
public:
  bool IsPairKey(int);
  int NumPairs() const;
  int NumJGroups() const;
  int IsPair(PPair);
  int IsJGroup(JGroup);
  int GetPairNumFromKey(int);
  int Fill(const Config&);
  int ReadECFile(std::string);
  int GetMaxLValue() const;
  void Initialize(const Config&);
  void AddPair(PPair);
  void AddJGroup(JGroup);
  void PrintNuc(const Config&);
  void TransformIn(bool);
  void SortPathways();
  void PrintPathways(const Config&);
  void CalcBoundaryConditions();
  void PrintBoundaryConditions(const Config&);
  void CalcAngularDists(int);
  void PrintAngularDists(const Config&);
  void FillMnParams(ROOT::Minuit2::MnUserParameters&);
  void FillCompoundFromParams(const vector_r &);
  void TransformOut(bool);
  void PrintTransformParams(std::string);
  void SetMaxLValue(int);
  complex CalcExternalWidth(JGroup*, ALevel*,AChannel*,bool);
  PPair *GetPair(int);
  JGroup *GetJGroup(int);
  CNuc *Clone() const;
private:
  std::vector<PPair> pairs_;
  std::vector<JGroup> jgroups_;
  int maxLValue_;
};

extern double DoubleFactorial(int);

#endif
