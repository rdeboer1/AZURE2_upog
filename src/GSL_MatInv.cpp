#include <gsl/gsl_linalg.h>
#include <gsl/gsl_matrix_complex_double.h>
#include <complex>
#include <vector>
#include <iostream>

std::vector<std::vector<std::complex<double> > > MatInv(const std::vector<std::vector<std::complex<double> > > &A) {
  gsl_complex x;
  gsl_matrix_complex * m = gsl_matrix_complex_alloc (A.size(), A.size());
  for(int i=0;i<A.size();i++) {
    for(int ii=0;ii<A.size();ii++) {
      GSL_SET_COMPLEX(&x,real(A[i][ii]),imag(A[i][ii]));
      gsl_matrix_complex_set (m,i,ii,x);
    }
  }
  int psign;
  gsl_permutation *p = gsl_permutation_alloc(A.size());
  gsl_matrix_complex * mi = gsl_matrix_complex_alloc (A.size(), A.size());
  gsl_linalg_complex_LU_decomp(m,p,&psign);
  gsl_linalg_complex_LU_invert(m,p,mi);

  std::vector<std::vector<std::complex<double> > > AI;
  std::vector<std::complex<double> > AI_row;
  for(int i=0;i<A.size();i++) {
    AI.push_back(AI_row);
    for(int ii=0;ii<A.size();ii++) {
      x=gsl_matrix_complex_get(mi,i,ii);
      std::complex<double> inv(GSL_REAL(x),GSL_IMAG(x));
      AI[i].push_back(inv);
    }
  }

  gsl_matrix_complex_free(m);
  gsl_matrix_complex_free(mi);
  gsl_permutation_free(p);
  return AI;
}
