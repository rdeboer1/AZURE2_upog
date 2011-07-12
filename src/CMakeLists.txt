set(AZURE2_SOURCES AChannel.cpp CoulFunc.cpp GenMatrixFunc.cpp KLGroup.cpp ALevel.cpp Decay.cpp GSL_AngCoeff.cpp MGroup.cpp AMatrixFunc.cpp  DoubleFactorial.cpp  GSL_Diagonalize.cpp  NFIntegral.cpp AZURE2.cpp ECIntegral.cpp GSL_MatInv.cpp PPair.cpp AZURECalc.cpp ECMGroup.cpp GSL_PEshift_dE.cpp ReactionRate.cpp AZUREMain.cpp EData.cpp GSL_ShftFunc.cpp RMatrixFunc.cpp AZUREOutput.cpp  EDataIterator.cpp    GSL_ShftFunc_dE.cpp  TargetEffect.cpp AZUREParams.cpp EPoint.cpp Interference.cpp CNuc.cpp Equation.cpp JGroup.cpp Config.cpp ESegment.cpp KGroup.cpp)

include_directories(${AZURE2_SOURCE_DIR}/include)

if (APPLE)
set(AZURE2_PROPERTIES MACOSX_BUNDLE)
endif (APPLE)

add_executable (AZURE2 ${AZURE2_PROPERTIES} ${AZURE2_SOURCES})
target_link_libraries (AZURE2 ${EXTRA_LIBS} ${GSL_LIBRARIES} ${MINUIT2_LIBRARY} readline ncurses)

install(TARGETS AZURE2 DESTINATION ${AZURE2_BINARY_DIR})
