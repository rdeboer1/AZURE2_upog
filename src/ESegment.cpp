#include "CNuc.h"
#include "DataLine.h"
#include "EData.h"
#include "ESegment.h"
#include "ExtrapLine.h"
#include "SegLine.h"

/*!
 * This constructor is used if the segment contains actual experimental data.  The segment
 * is created with reference to an entry in the segments data file.
 */

ESegment::ESegment(struct SegLine segLine) {
  entrancekey_=segLine.entrancekey;
  exitkey_=segLine.exitkey;
  min_e_=segLine.min_e;
  max_e_=segLine.max_e;
  min_a_=segLine.min_a; 
  max_a_=segLine.max_a;
  e_step_=0.0;
  a_step_=0.0;
  segment_chi_squared_=0.0;
  if(segLine.diff==1) isdifferential_=true;
  else isdifferential_=false;
  if(segLine.diff==2) {
    isphase_=true;
    j_=segLine.j;
    l_=segLine.l;
  } else {
    isphase_=false;
    j_=0.0;
    l_=0;
  }
  datafile_=segLine.datafile;
  targetEffectNum_=0;
  isTargetEffect_=false;
}

/*!
 * This constructor is used if the segment contains no actual data, and the points must be created
 * (such as in an extrapolation).  The segment is created with reference to an entry in the segments
 * extrapolation file.
 */

ESegment::ESegment(struct ExtrapLine extrapLine) {
  entrancekey_=extrapLine.entrancekey;
  exitkey_=extrapLine.exitkey;
  min_e_=extrapLine.min_e;
  max_e_=extrapLine.max_e;
  min_a_=extrapLine.min_a; 
  max_a_=extrapLine.max_a;
  e_step_=extrapLine.e_step;
  a_step_=extrapLine.a_step;
  segment_chi_squared_=0.0;
  if(extrapLine.diff==1) isdifferential_=true;
  else isdifferential_=false;
  if(extrapLine.diff==2) {
    isphase_=true;
    j_=extrapLine.j;
    l_=extrapLine.l;
  } else {
    isphase_=false;
    j_=0.0;
    l_=0;
  }
  datafile_="";
  targetEffectNum_=0;
  isTargetEffect_=false;
}

/*!
 * Returns true if a point is with the specified angle and energy ranges of a segment, otherwise
 * returns false.
 */

bool ESegment::IsInSegment(EPoint point) {
  bool b=false;
  if(point.GetLabEnergy()>=this->GetMinEnergy()&&
     point.GetLabEnergy()<=this->GetMaxEnergy()) {
    if(this->IsDifferential()) {
      if(point.GetLabAngle()>=this->GetMinAngle()&&
	 point.GetLabAngle()<=this->GetMaxAngle()) b=true;
    } else b=true;
  }
  return b;
}

/*!
 * Returns true if the segment is differential cross section, otherwise returns false.
 */

bool ESegment::IsDifferential() const {
  return isdifferential_;
}

/*!
 * Returns true if the segment is phase shift, otherwise returns false.
 */

bool ESegment::IsPhase() const {
  return isphase_;
}

/*!
 * Returns true if the segment has a corresponding TargetEffect object, otherwise
 * returns false.
 */

bool ESegment::IsTargetEffect() const {
  return isTargetEffect_;
}

/*!
 * Returns the number of data point objects in the segment.
 */

int ESegment::NumPoints() const {
  return points_.size();
}

/*!
 * Returns the entrance particle pair key of the segment.
 */

int ESegment::GetEntranceKey() const {
  return entrancekey_;
}

/*!
 * Returns the exit particle pair key of the segment.
 */

int ESegment::GetExitKey() const {
  return exitkey_;
}

/*!
 * Fills the segment with points from the specified data file according to the
 * maximum and minimum energy and angle ranges.  The data is assumed to be entirely
 * in the lab frame, and conversions are performed to the center of mass frame.
 */

int ESegment::Fill(CNuc *theCNuc, EData *theData) {
  std::string infile=this->GetDataFile();
  std::ifstream in(infile.c_str());
  if(!in) return -1;
  while(!in.eof()) {
    DataLine line=ReadDataLine(in);
    if(!in.eof()) {
      EPoint NewEPoint(line,this);
      if(this->IsInSegment(NewEPoint)) {
	this->AddPoint(NewEPoint);
	PPair *entrancePair=theCNuc->GetPair(theCNuc->GetPairNumFromKey(this->GetEntranceKey()));
	PPair *exitPair=theCNuc->GetPair(theCNuc->GetPairNumFromKey(this->GetExitKey()));
	this->GetPoint(this->NumPoints())->SetParentData(theData);
	this->GetPoint(this->NumPoints())->ConvertLabEnergy(entrancePair);
	if(exitPair->GetPType()==0&&this->IsDifferential()&&!this->IsPhase()) {
	  if(this->GetEntranceKey()==this->GetExitKey()) {
	    this->GetPoint(this->NumPoints())->ConvertLabAngle(entrancePair);
	  } else {
	    this->GetPoint(this->NumPoints())->ConvertLabAngle(entrancePair,exitPair);
	  }
	  this->GetPoint(this->NumPoints())->ConvertCrossSection();
	}
      }
    }
  }
  in.close();
  return 0;
}

/*!
 * Returns the orbital angular momentum value for the segment.  Applies only if the segment is phase shift.
 */

int ESegment::GetL() const {
  return l_;
}

/*!
 * Returns the position of the corresponding TargetEffect object in the vector
 *  of the parent EData object.
 */

int ESegment::GetTargetEffectNum() const {
  return targetEffectNum_;
}

/*! 
 * Returns the segment key for the current ESegment object.  The segment key is the order of 
 * the segment specified in the input file, INCLUDING non-active segments.
 */

int ESegment::GetSegmentKey() const {
  return segmentKey_;
}

/*!
 * Returns the minimum energy of the segment (lab frame).
 */

double ESegment::GetMinEnergy() const {
  return min_e_;
}

/*!
 * Returns the maximum energy of the segment (lab frame).
 */

double ESegment::GetMaxEnergy() const {
  return max_e_;
}

/*!
 * Returns the minimum angle of the segment (lab frame).
 */

double ESegment::GetMinAngle() const {
  return min_a_;
}

/*!
 * Returns the maximum angle of the segment (lab frame).
 */

double ESegment::GetMaxAngle() const {
  return max_a_;
}

/*!
 * Returns the chi-squared value of the segment after the fitting process.
 */

double ESegment::GetSegmentChiSquared() const {
  return segment_chi_squared_;
}

/*!
 * Returns the energy step to take when creating points in a segment.  Only applies for extrapolation segments.
 */

double ESegment::GetEStep() const {
  return e_step_;
}

/*!
 * Returns the angle step to take when creating points in a segment.  Only applies for extrapolation segments.
 */

double ESegment::GetAStep() const {
  return a_step_;
}

/*!
 * Returns the total spin of the segment.  Only applies if the segment is phase shift.
 */

double ESegment::GetJ() const {
  return j_;
}

/*!
 * Returns the name of the data file from which to read.
 */

std::string ESegment::GetDataFile() const {
  return datafile_;
}

/*!
 * Adds a data point to the segment.
 */

void ESegment::AddPoint(EPoint point) {
  points_.push_back(point);
}

/*!
 * Sets the chi squared value for the segment during the fitting process.  
 */

void ESegment::SetSegmentChiSquared(double chiSquared) {
  segment_chi_squared_=chiSquared;
}

/*!
 * Sets the position of the corresponding TargetEffect object in the vector
 * of the parent EData object.
 */

void ESegment::SetTargetEffectNum(int targetEffectNum) {
  targetEffectNum_=targetEffectNum;
  isTargetEffect_=true;
}

/*! 
 * Sets the segment key for the current ESegment object.
 */

void ESegment::SetSegmentKey (int segmentKey) {
  segmentKey_=segmentKey;
}

/*!
 * Returns a pointer to the data point object specified by a position in the EPoint vector.
 */

EPoint *ESegment::GetPoint(int pointNum) {
  EPoint *b=&points_[pointNum-1];
  return b;
}
