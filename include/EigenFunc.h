#ifndef DIAGONALIZE_H
#define DIAGONALIZE_H

#include "Constants.h"

class EigenFunc {
 public:
  EigenFunc(const matrix_r&);
  EigenFunc(const matrix_r&, const std::vector<vector_r >&);
  const vector_r& eigenvalues() const {return eigenvalues_;};
  const matrix_r& eigenvectors() const {return eigenvectors_;};
 private:
  vector_r eigenvalues_;
  matrix_r eigenvectors_;
};
  
#endif
