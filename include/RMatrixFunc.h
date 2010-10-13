#ifndef RMATRIXFUNC_H
#define RMATRIXFUNC_H

#include "GenMatrixFunc.h"

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
  void FillMatrices(EPoint*,bool isBrune=false);
  void InvertMatrices();
  void CalculateTMatrix(EPoint*);
  /*!
   * Instantiated in the parent class.
   */
  void CalculateCrossSection();

  double GetRMatrixElement(int,int,int) const;
  complex GetRLMatrixElement(int,int,int) const;
  complex GetRLInvMatrixElement(int,int,int) const;
  complex GetRLInvRMatrixElement(int,int,int) const;
  matrix_c *GetJSpecRLMatrix(int);
  void AddRMatrixElement(int,int,int,double);
  void AddRLMatrixElement(int,int,int,complex);
  void AddRLInvMatrix(matrix_c);
  void AddRLInvRMatrixElement(int,int,int,complex);
 private:
  CNuc *compound_;
  vector_matrix_r r_matrices_;
  vector_matrix_c rl_matrices_;
  vector_matrix_c rl_inv_matrices_;
  vector_matrix_c rl_inv_r_matrices_;
};

#endif
