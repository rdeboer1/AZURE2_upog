#ifndef ESEGMENT_H
#define ESEGMENT_H

#include "EPoint.h"
#include "SegLine.h"
#include "ExtrapLine.h"

///An AZURE data segment

/*!
 * An AZURE data segment is specified by an entrance and exit particle pair key, as well as a range of 
 * energy and angle value and a data file name. The segment also contains flags specifing they type of data point it contains. The ESegment object is the container object for a vector of EData objects.
 */

class ESegment {
 public:
  ESegment(SegLine); 
  ESegment(ExtrapLine); 
  bool IsInSegment(EPoint);
  bool IsDifferential() const;
  bool IsPhase() const;
  int NumPoints() const;
  int GetEntranceKey() const;
  int GetExitKey() const;
  int Fill(CNuc*);
  int GetL() const;
  double GetMinEnergy() const;
  double GetMaxEnergy() const;
  double GetMinAngle() const;
  double GetMaxAngle() const;
  double GetSegmentChiSquared() const;
  double GetEStep() const;
  double GetAStep() const;
  double GetJ() const;
  std::string GetDataFile() const;
  void AddPoint(EPoint);
  void SetSegmentChiSquared(double);
  EPoint *GetPoint(int);
 private:
  bool isdifferential_;
  bool isphase_;
  int entrancekey_;
  int exitkey_;
  int l_;
  double min_e_;
  double max_e_;
  double min_a_;
  double max_a_;
  double e_step_;
  double a_step_;
  double segment_chi_squared_;
  double j_;
  std::string datafile_;
  std::vector<EPoint> points_;
};

#endif
