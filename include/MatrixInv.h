#ifndef MATRIXINV_H
#define MATRIXINV_H

#include "Constants.h"

class MatrixInv {
 public:
  MatrixInv(const matrix_c&);
  const matrix_c& inverse() const {return inverse_;};
 private:
 matrix_c inverse_;
};

#endif
