cmake_minimum_required (VERSION 2.8)
project (AZURE2)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package (gsl REQUIRED)
include_directories(${GSL_INCLUDE_DIR})
find_package (Minuit2 REQUIRED)
include_directories(${MINUIT2_INCLUDE_DIR})
find_package (OpenMP REQUIRED)
set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS})

option (USE_EXT_COUL
       "Use non-gsl Coulomb functions" OFF)
option (BUILD_GUI 
       "Build AZURE2 with graphical setup program" ON)

set (EXTRA_LIBS "")

if (USE_EXT_COUL) 
include_directories(${AZURE2_SOURCE_DIR}/coul/include)
add_subdirectory (coul)
add_definitions(-DEXT_COUL)
set (EXTRA_LIBS ${EXTRA_LIBS} coul)
endif (USE_EXT_COUL)

if (BUILD_GUI)
add_subdirectory (gui)
set(EXTRA_LIBS ${EXTRA_LIBS} ${AZURESETUP_LIBRARIES})
add_definitions(-DGUI_BUILD)
endif (BUILD_GUI)

add_subdirectory (src)

