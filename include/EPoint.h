#ifndef EPOINT_H
#define EPOINT_H

#include "DataLine.h"
#include "RMatrixFunc.h"
#include "AMatrixFunc.h"

///A container structure for a reference to a data point.

struct EnergyMap {
  int segment;
  int point;
};

class ESegment;

///An AZURE data point

/*!
 * A data point object in AZURE consists of a defined entrance and exit pair, an energy, an angle,
 * measured cross section and uncertainty, s-factor conversions, and
 * several flags that determine the type of data (angle integrated or differential) to be analysed.
 */

class EPoint {
 public:
  EPoint(DataLine, ESegment*);
  EPoint(double, double, ESegment*);
  EPoint(double, double, int, int, bool, bool, double, int);
  bool IsDifferential() const;
  bool IsPhase() const;
  bool IsMapped() const;
  int GetEntranceKey() const;
  int GetExitKey() const;
  int GetMaxLOrder() const;
  int GetL() const;
  int NumLocalMappedPoints() const;
  double GetLabAngle() const;
  double GetCMAngle() const;
  double GetLabEnergy() const;
  double GetCMEnergy() const;
  double GetLegendreP(int) const;
  double GetLabCrossSection() const;
  double GetCMCrossSection() const;
  double GetLabCrossSectionError() const;
  double GetCMCrossSectionError() const;
  double GetGeometricalFactor() const;
  double GetFitCrossSection() const;
  double GetSFactorConversion() const;
  double GetSqrtPenetrability(int,int) const;
  double GetJ() const;
  std::complex<double> GetLoElement(int,int) const;
  std::complex<double> GetExpCoulombPhase(int,int) const;
  std::complex<double> GetExpHardSpherePhase(int,int) const;
  std::complex<double> GetCoulombAmplitude() const;
  std::complex<double> GetECAmplitude(int,int) const;
  EnergyMap GetMap() const;
  void Initialize(CNuc*,const Config&);
  void ConvertLabEnergy(PPair*);
  void ConvertLabAngle(PPair*);
  void ConvertLabAngle(PPair*,PPair*);
  void ConvertCrossSection();
  void AddLegendreP(double);
  void SetGeometricalFactor(double);
  void SetFitCrossSection(double);
  void SetSFactorConversion(double);
  void CalcLegendreP(int);
  void CalcEDependentValues(CNuc*);
  void AddLoElement(int,int,std::complex<double>);
  void AddSqrtPenetrability(int,int,double);
  void AddExpCoulombPhase(int,int,std::complex<double>);
  void AddExpHardSpherePhase(int,int,std::complex<double>);
  void CalcCoulombAmplitude(CNuc*);
  void SetCoulombAmplitude(std::complex<double>);
  void CalculateECAmplitudes(CNuc*);
  void AddECAmplitude(int,int,std::complex<double>);
  void Calculate(CNuc*,const Config &configure);
  void SetMap(int,int);
  void AddLocalMappedPoint(EPoint*);
  void CalculateTargetEffects(CNuc*,const Config &);
  EPoint* GetLocalMappedPoint(int) const;
 private:
  int entrance_key_;
  int exit_key_;
  double cm_angle_;
  double lab_angle_;
  double cm_energy_;
  double lab_energy_;
  double cm_crosssection_;
  double cm_dcrosssection_;
  double lab_crosssection_;
  double lab_dcrosssection_;
  double geofactor_;
  double fitcrosssection_;
  double sfactorconv_;
  bool is_differential_;
  bool is_phase_;
  bool is_mapped_;
  double j_value_;
  int l_value_;
  struct EnergyMap energy_map_;
  std::complex<double> coulombamplitude_;
  std::vector<double> legendreP_;
  std::vector<std::vector<std::complex<double> > > lo_elements_;
  std::vector<std::vector<double> > penetrabilities_;
  std::vector<std::vector<std::complex<double> > > coulombphase_;
  std::vector<std::vector<std::complex<double> > > hardspherephase_;
  std::vector<std::vector<std::complex<double> > > ec_amplitudes_;
  std::vector<EPoint*> local_mapped_points_;
};

#endif
