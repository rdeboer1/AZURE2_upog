#ifndef DIAGONALIZE_H
#define DIAGONALIZE_H

#include <vector>

struct EigenSolve {
  std::vector<double> eigenvalues;
  std::vector<std::vector<double> > eigenvectors;
};
  
extern struct EigenSolve Diagonalize(const std::vector<std::vector<double> >&);
extern struct EigenSolve Solve(const std::vector<std::vector<double> >&,const std::vector<std::vector<double> >&);

#endif
