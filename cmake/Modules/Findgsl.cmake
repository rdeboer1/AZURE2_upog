#Note, the path to GSL also needs to be defined so that it can find the shared library files at run time. This has been done in my .login file by the command
#setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:/opt/crc/scilib/gsl/1.15/gnu-4.6.2/lib
#Of course if I change versions of gsl I will need to update this both here and in my .login file


find_path(GSL_INCLUDE_DIR gsl/gsl_cblas.h
          HINTS $ENV{HOME}/local/include $ENV{HOME}/include /opt/crc/scilib/gsl/1.15/gnu-4.6.2/include) # /opt/crc/scilib/gsl/1.15/gnu-4.6.2/include
# /opt/crc/scilib/gsl/1.15/gnu-4.6.2/include

find_library(GSL_LIBRARY NAMES gsl 
             HINTS $ENV{HOME}/local/lib $ENV{HOME}/lib /opt/crc/scilib/gsl/1.15/gnu-4.6.2/lib) # /opt/crc/scilib/gsl/1.15/gnu-4.6.2/lib
# /opt/crc/scilib/gsl/1.15/gnu-4.6.2/lib

find_library(GSLCBLAS_LIBRARY NAMES gslcblas 
             HINTS $ENV{HOME}/local/lib $ENV{HOME}/lib /opt/crc/scilib/gsl/1.15/gnu-4.6.2/lib) # /opt/crc/scilib/gsl/1.15/gnu-4.6.2/lib
# /opt/crc/scilib/gsl/1.15/gnu-4.6.2/lib

set(GSL_LIBRARIES ${GSL_LIBRARY} ${GSLCBLAS_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(gsl  DEFAULT_MSG
                                  GSL_LIBRARY GSL_INCLUDE_DIR)

mark_as_advanced(GSL_INCLUDE_DIR GSL_LIBRARY GSLCBLAS_LIBRARY)
