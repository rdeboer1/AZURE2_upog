#ifndef DIAGONALIZE_H
#define DIAGONALIZE_H

#include "Constants.h"

struct EigenSolve {
  vector_r eigenvalues;
  matrix_r eigenvectors;
};
  
extern struct EigenSolve Diagonalize(const matrix_r&);
extern struct EigenSolve Solve(const matrix_r&,const std::vector<vector_r >&);

#endif
