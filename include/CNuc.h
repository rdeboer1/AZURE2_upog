#ifndef CNUC_H
#define CNUC_H

#include <string>
#include "JGroup.h"
#include "PPair.h"

namespace ROOT {
  namespace Minuit2 {
    class MnUserParameters;
  }
}

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
  int Fill(const struct Config&);
  int ReadECFile(std::string);
  int GetMaxLValue() const;
  void Initialize(const struct Config&);
  void AddPair(PPair);
  void AddJGroup(JGroup);
  void PrintNuc(const struct Config&);
  void TransformIn(const struct Config&);
  void SortPathways();
  void PrintPathways(const struct Config&);
  void CalcBoundaryConditions();
  void PrintBoundaryConditions(const struct Config&);
  void CalcAngularDists(int);
  void PrintAngularDists(const struct Config&);
  void FillMnParams(ROOT::Minuit2::MnUserParameters&);
  void FillCompoundFromParams(const vector_r &);
  void TransformOut(const struct Config&);
  void PrintTransformParams(std::string);
  void SetMaxLValue(int);
  void CalcShiftFunctions();
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
