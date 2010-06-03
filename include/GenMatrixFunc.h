#ifndef GENMATRIXFUNC_H
#define GENMATRIXFUNC_H

#include "CNuc.h"

class EPoint;
/// A temporaray T-Matrix structure

/*!
 * The TempTMatrix structure is used to coherently add T-matrix elements from 
 * pathways  with like \f$ J,l,l' \f$ values for the calculation of angle integrated cross section.
 * This is primarly used to facilitate the interference between internal and external pathways.
 */

struct TempTMatrix {
  ///Total spin value of temporary matrix element
  double jValue;
  /// Entrance orbital angular momentum for temporary matrix element
  int lValue;
  /// Exit orbital angular momentum for temporary matrix element
  int lpValue;
  ///Value of temporary matrix element
  std::complex<double> TMatrix;
};


///A generalized function class to calculate cross sections

/*!
 * The GenMatrixFunc function class is the general form of the function used 
 * to calculate cross section from R-Matrix parameters.  It is the parent class of
 * AMatrixFunc and RMatrixFunc.
 */

class GenMatrixFunc {
 public:
  GenMatrixFunc() {};
  virtual ~GenMatrixFunc(){};
  /*!
   *This virtual function in instantiated in the child class.
   */
  virtual void ClearMatrices()=0;
  /*!
   *This virtual function in instantiated in the child class.
   */
  virtual void FillMatrices(EPoint*)=0;
  /*!
   *This virtual function in instantiated in the child class.
   */
  virtual void InvertMatrices()=0;
  /*!
   *This virtual function in instantiated in the child class.
   */
  virtual void CalculateTMatrix(EPoint*)=0;
  void CalculateCrossSection(EPoint*);
  void NewTempTMatrix(TempTMatrix);
  void AddToTempTMatrix(int,std::complex<double>);
  void ClearTempTMatrices();
  void AddTMatrixElement(int,int,std::complex<double>);
  void AddECTMatrixElement(int,int,std::complex<double>);
  int IsTempTMatrix(double,int,int);
  int NumTempTMatrices() const;
  TempTMatrix *GetTempTMatrix(int);
  std::complex<double> GetTMatrixElement(int,int) const;
  std::complex<double> GetECTMatrixElement(int,int) const;

  /*!
   *This virtual function in instantiated in the child class.
   */
  virtual CNuc *compound() const = 0;
 protected:
  ///Vector of internal T-matrix elements accessable to child class.
  std::vector<std::vector<std::complex<double > > > tmatrix_;
  ///Vector of external T-matrix elements accessable to child class.
  std::vector<std::vector<std::complex<double > > > ec_tmatrix_;
 private:
  std::vector<TempTMatrix> temp_t_matrices_;
};

#endif
