#ifndef EDATA_H
#define EDATA_H

#include "ESegment.h"
#include "TargetEffect.h"

class CNuc;

///An AZURE data object

/*!
 * The EData object is the top level data object in AZURE.  It is the container object for a vector of ESegment objects.
 */

class EData {
 public:
  EData();
  int NumSegments() const;
  int Fill(const struct Config&,CNuc*);
  int MakePoints(const struct Config&,CNuc*);
  int Iterations() const;
  int NumTargetEffects() const;
  bool IsFit() const;
  bool IsErrorAnalysis() const;
  bool IsSegmentKey(int);
  void SetFit(bool);
  void SetErrorAnalysis(bool);
  void Iterate();
  void ResetIterations();
  void Initialize(CNuc*,const struct Config&);
  void AddSegment(ESegment);
  void PrintData(const struct Config&);
  void CalcLegendreP(int);
  void PrintLegendreP(const struct Config&);
  void CalcEDependentValues(CNuc*);
  void PrintEDependentValues(const struct Config&,CNuc*);
  void CalcCoulombAmplitude(CNuc*);
  void PrintCoulombAmplitude(const struct Config&,CNuc*); 
  void WriteOutputFiles(const struct Config&);
  void CalculateECAmplitudes(CNuc*,const struct Config&);
  void MapData();
  void AddTargetEffect(TargetEffect);
  void ReadTargetEffectsFile(std::string);
  ESegment *GetSegment(int);
  ESegment *GetSegmentFromKey(int);
  EData *Clone() const;
  TargetEffect *GetTargetEffect(int);
 private:
  std::vector<TargetEffect> targetEffects_;
  std::vector<ESegment> segments_;
  int iterations_;
  bool isFit_;
  bool isErrorAnalysis_;
};

#endif
