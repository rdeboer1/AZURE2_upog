depsdir = .deps

MAKEDEPEND = $(CXX) $(CPPFLAGS) -M $< | sed 's/$@/& $(depsdir)\/$*.d/g' > $(depsdir)/$*.d

SRCS =  AZUREMain.cpp\
	AChannel.cpp\
	ALevel.cpp\
	CNuc.cpp\
	Config.cpp\
	Decay.cpp\
	EData.cpp\
	EPoint.cpp\
	ESegment.cpp\
	GSL_AngCoeff.cpp\
	GSL_ShftFunc.cpp\
	GSL_ShftFunc_dE.cpp\
	Interference.cpp\
	JGroup.cpp\
	KGroup.cpp\
	KLGroup.cpp\
	MGroup.cpp\
	PPair.cpp\
	AZURECalc.cpp\
	GenMatrixFunc.cpp\
	RMatrixFunc.cpp\
	GSL_MatInv.cpp\
	AZUREOutput.cpp\
	AMatrixFunc.cpp\
	ECMGroup.cpp\
	ECIntegral.cpp\
	NFIntegral.cpp\
	GSL_Diagonalize.cpp\
	GSL_PEshift_dE.cpp\
	DoubleFactorial.cpp\
	ReactionRate.cpp\
	AZUREParams.cpp\
	Equation.cpp\
	TargetEffect.cpp\
	CoulFunc.cpp\
	EDataIterator.cpp\
        AZURE2.cpp

../AZURE2 : $(SRCS:.cpp=.o)
	$(LD) $(LFLAGS) -o $@ $(SRCS:.cpp=.o) $(LIBS) 

%.o : %.cpp | $(depsdir)
	@$(MAKEDEPEND)
	$(CXX)  $(CPPFLAGS) -c -o $@ $<

-include $(SRCS:%.cpp=$(depsdir)/%.d)

$(depsdir) :
	@(mkdir $(depsdir))

.PHONY : clean
clean :
	-rm -f $(SRCS:%.cpp=%.o);
	-rm -rf $(depsdir)

