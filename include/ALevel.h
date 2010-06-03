#ifndef ALEVEL_H
#define ALEVEL_H

#include <vector>
#include <complex>
#include "NucLine.h"

///An AZURE level object.

/*!
 * An R-matrix level  represents a specific eigenstate of the compound nucleus.
 */

class ALevel {
 public:
  ALevel(NucLine);
  ALevel(double);
  bool IsInRMatrix() const;
  bool EnergyFixed() const;
  bool ChannelFixed(int) const;
  int NumNFIntegrals() const;
  int GetTransformIterations() const;
  double GetE() const;
  double GetGamma(int) const;
  double GetFitGamma(int) const;
  double GetFitE() const;
  double GetNFIntegral(int) const;
  double GetSqrtNFFactor() const;
  double GetECConversionFactor(int) const;
  double GetTransformGamma(int) const;
  double GetTransformE() const;
  double GetBigGamma(int) const;
  std::complex<double> GetExternalGamma(int) const;
  void AddGamma(NucLine);
  void AddGamma(double);
  void SetGamma(int,double);
  void SetE(double);
  void SetFitGamma(int,double);
  void SetFitE(double);
  void AddNFIntegral(double);
  void SetSqrtNFFactor(double);
  void AddECConversionFactor(double);
  void SetTransformGamma(int,double);
  void SetTransformE(double);
  void SetBigGamma(int,double);
  void SetTransformIterations(int);
  void SetExternalGamma(int,std::complex<double>);
 private:
  bool isinrmatrix_;
  bool energyfixed_;
  int transform_iter_;
  double level_e_;
  double fitlevel_e_;
  double sqrt_nf_factor_;
  double transform_e_;
  std::vector<bool> channelfixed_;
  std::vector<double> gammas_;
  std::vector<double> fitgammas_;
  std::vector<double> nf_integrals_;
  std::vector<double> ec_conv_factors_;
  std::vector<double> transform_gammas_;
  std::vector<double> big_gammas_;
  std::vector<std::complex<double> > external_gammas_;
};

#endif
