#ifndef ALEVEL_H
#define ALEVEL_H

#include "Constants.h"
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
  complex GetExternalGamma(int) const;
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
  void SetExternalGamma(int,complex);
 private:
  bool isinrmatrix_;
  bool energyfixed_;
  int transform_iter_;
  double level_e_;
  double fitlevel_e_;
  double sqrt_nf_factor_;
  double transform_e_;
  std::vector<bool> channelfixed_;
  vector_r gammas_;
  vector_r fitgammas_;
  vector_r nf_integrals_;
  vector_r ec_conv_factors_;
  vector_r transform_gammas_;
  vector_r big_gammas_;
  vector_c external_gammas_;
};

#endif
