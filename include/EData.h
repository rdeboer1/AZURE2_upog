#ifndef EDATA_H
#define EDATA_H

#include "ESegment.h"
#include "AZUREOutput.h"

///An AZURE data object

/*!
 * The EData object is the top level data object in AZURE.  It is the container object for a vector of ESegment objects.
 */

class EData {
 public:
  EData();
  int NumSegments() const;
  int Fill(std::string,CNuc*);
  int MakePoints(std::string,CNuc*);
  int Iterations() const;
  bool IsFit() const;
  void SetFit(bool);
  void Iterate();
  void Initialize(CNuc*,const Config&);
  void AddSegment(ESegment);
  void PrintData(const Config&);
  void CalcLegendreP(int);
  void PrintLegendreP(const Config&);
  void CalcEDependentValues(CNuc*);
  void PrintEDependentValues(const Config&,CNuc*);
  void CalcCoulombAmplitude(CNuc*);
  void PrintCoulombAmplitude(const Config&,CNuc*); 
  void WriteOutputFiles(const Config&);
  void CalculateECAmplitudes(CNuc*,const Config&);
  void MapData();
  ESegment *GetSegment(int);
  EData *Clone() const;
 private:
  std::vector<ESegment> segments_;
  int iterations_;
  bool isFit_;
};

#endif
