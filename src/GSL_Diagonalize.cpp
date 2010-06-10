#include "Diagonalize.h"
#include <gsl/gsl_eigen.h>
#include <math.h>

struct EigenSolve Diagonalize(const matrix_r &A) {

  gsl_matrix * m = gsl_matrix_alloc(A.size(),A.size());
  for(int i=0;i<A.size();i++) {
    for(int ii=0;ii<A.size();ii++) {
      gsl_matrix_set (m,i,ii,A[i][ii]);
    }
  }

  gsl_vector * eval = gsl_vector_alloc (A.size());
  gsl_matrix * evec = gsl_matrix_alloc (A.size(), A.size());
  gsl_eigen_symmv_workspace * w = gsl_eigen_symmv_alloc (A.size());

  gsl_eigen_symmv (m,eval,evec,w);
  gsl_eigen_symmv_sort (eval,evec,GSL_EIGEN_SORT_VAL_ASC);

  struct EigenSolve result;
  for(int i=0;i<A.size();i++) {
    result.eigenvalues.push_back(gsl_vector_get(eval,i));
    vector_r tempRow;
    result.eigenvectors.push_back(tempRow);
    for(int ii=0;ii<A.size();ii++) {
      result.eigenvectors[i].push_back(gsl_matrix_get(evec,i,ii));
    }
  }
  
  gsl_vector_free (eval);
  gsl_matrix_free (evec);
  gsl_matrix_free (m);
  gsl_eigen_symmv_free (w);

  return result;
}

struct EigenSolve Solve(const matrix_r &A, const std::vector<vector_r > &B) {
  gsl_matrix * n = gsl_matrix_alloc(A.size(),A.size());
  gsl_matrix * m = gsl_matrix_alloc(B.size(),B.size());
  for(int i=0;i<A.size();i++) {
    for(int ii=0;ii<A.size();ii++) {
      gsl_matrix_set (n,i,ii,A[i][ii]);
      gsl_matrix_set (m,i,ii,B[i][ii]);
    }
  }
  
  gsl_vector * eval = gsl_vector_alloc (A.size());
  gsl_matrix * evec = gsl_matrix_alloc (A.size(), A.size());

  gsl_eigen_gensymmv_workspace * w = gsl_eigen_gensymmv_alloc (A.size());

  gsl_eigen_gensymmv(n,m,eval,evec,w);
  gsl_eigen_gensymmv_sort (eval,evec,GSL_EIGEN_SORT_VAL_ASC);

  struct EigenSolve result;
  for(int i=0;i<A.size();i++) {
    result.eigenvalues.push_back(gsl_vector_get(eval,i));
    vector_r tempRow;
    result.eigenvectors.push_back(tempRow);
    for(int ii=0;ii<A.size();ii++) {
      result.eigenvectors[i].push_back(gsl_matrix_get(evec,i,ii));
    }
  }

  vector_r tempNormVec;
  for(int i=0;i<A.size();i++) tempNormVec.push_back(0.0);

  for(int i=0;i<A.size();i++) {
    for(int ii=0;ii<A.size();ii++) {
      double sum=0.0;
      for(int iii=0;iii<A.size();iii++) {
	sum+=B[ii][iii]*result.eigenvectors[iii][i];
      }
      tempNormVec[ii]=sum;
    }
    double sum=0.0;
    for(int ii=0;ii<A.size();ii++) {
      sum+=tempNormVec[ii]*result.eigenvectors[ii][i];
    }
    for(int ii=0;ii<A.size();ii++) {
      result.eigenvectors[ii][i]=result.eigenvectors[ii][i]/sqrt(sum);
    }
  }
  
  gsl_vector_free (eval);
  gsl_matrix_free (evec);
  gsl_matrix_free (m);
  gsl_matrix_free (n);
  gsl_eigen_gensymmv_free (w);

  return result;
}
