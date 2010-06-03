#ifndef AMATRIXFUNC_H
#define AMATRIXFUNC_H

#include "GenMatrixFunc.h"
#include "MatInv.h"

///A function class to calculate the T-Matrix using the A-Matrix

/*!
 * The AMatrixFunc function class calculates the T-Matrix for a given energy point using the compound
 * nucleus object.  The AMatrixFunc class is a child class of GenMatrixFunc, where the cross section is
 * calculated from the T-Matrix.
 */

class AMatrixFunc : public GenMatrixFunc {
 public:
  AMatrixFunc(CNuc*);
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

  std::complex<double> GetAMatrixElement(int,int,int) const;
  std::vector<std::vector<std::complex<double> > > *GetJSpecAInvMatrix(int);
  void AddAInvMatrixElement(int,int,int,std::complex<double>);
  void AddAMatrix(std::vector<std::vector<std::complex<double> > >);
 private:
  CNuc *compound_;
  std::vector<std::vector<std::vector<std::complex<double> > > > a_inv_matrices_;
  std::vector<std::vector<std::vector<std::complex<double> > > > a_matrices_;
};

#endif
