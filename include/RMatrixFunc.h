#ifndef RMATRIXFUNC_H
#define RMATRIXFUNC_H

#include "GenMatrixFunc.h"
#include "MatInv.h"

///A function class to calculate the T-Matrix using the R-Matrix

/*!
 * The RMatrixFunc function class calculates the T-Matrix for a given energy point using the compound
 * nucleus object.  The RMatrixFunc class is a child class of GenMatrixFunc, where the cross section is
 * calculated from the T-Matrix.
 */

class RMatrixFunc : public GenMatrixFunc {
 public:
  RMatrixFunc(CNuc*);
  /*!
   * Returns a pointer to the compound nucleus object.
   */
  CNuc *compound() const {return compound_;};

  void ClearMatrices();
  void FillMatrices(EPoint*);
  void InvertMatrices();
  void CalculateTMatrix(EPoint*);
  /*!
   * Instantiated in the parent class.
   */
  void CalculateCrossSection();

  double GetRMatrixElement(int,int,int) const;
  std::complex<double> GetRLMatrixElement(int,int,int) const;
  std::complex<double> GetRLInvMatrixElement(int,int,int) const;
  std::complex<double> GetRLInvRMatrixElement(int,int,int) const;
  std::vector<std::vector<std::complex<double> > > *GetJSpecRLMatrix(int);
  void AddRMatrixElement(int,int,int,double);
  void AddRLMatrixElement(int,int,int,std::complex<double>);
  void AddRLInvMatrix(std::vector<std::vector<std::complex<double> > >);
  void AddRLInvRMatrixElement(int,int,int,std::complex<double>);
 private:
  CNuc *compound_;
  std::vector<std::vector<std::vector<double> > > r_matrices_;
  std::vector<std::vector<std::vector<std::complex<double> > > > rl_matrices_;
  std::vector<std::vector<std::vector<std::complex<double> > > > rl_inv_matrices_;
  std::vector<std::vector<std::vector<std::complex<double> > > > rl_inv_r_matrices_;
};

#endif
