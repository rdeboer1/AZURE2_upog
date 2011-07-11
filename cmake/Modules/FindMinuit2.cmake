find_path(MINUIT2_INCLUDE_DIR Minuit2/MnUserFcn.h
          HINTS ${HOME}/local ${HOME})

find_library(MINUIT2_LIBRARY NAMES Minuit2
             HINTS ${HOME}/local/lib ${HOME}/lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(gsl  DEFAULT_MSG
                                  MINUIT2_LIBRARY MINUIT2_INCLUDE_DIR)

mark_as_advanced(MINUIT2_INCLUDE_DIR MINUIT2_LIBRARY)