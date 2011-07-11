find_path(GSL_INCLUDE_DIR gsl/gsl_cblas.h
          HINTS ${HOME}/local ${HOME})

find_library(GSL_LIBRARY NAMES gsl 
             HINTS ${HOME}/local/lib ${HOME}/lib)

find_library(GSLCBLAS_LIBRARY NAMES gslcblas 
             HINTS ${HOME}/local/lib ${HOME}/lib)

set(GSL_LIBRARIES ${GSL_LIBRARY} ${GSLCBLAS_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(gsl  DEFAULT_MSG
                                  GSL_LIBRARY GSL_INCLUDE_DIR)

mark_as_advanced(GSL_INCLUDE_DIR GSL_LIBRARY GSLCBLAS_LIBRARY)