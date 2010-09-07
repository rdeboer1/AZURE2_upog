SHELL = /bin/sh

srcdir = src
builddir = build

CXX = icc
CPPFLAGS = -Iinclude 
CPPFLAGS +=  -I../include -L../lib
LIBS = -lgsl -lgslcblas -lMinuit2 -lreadline -lglib-2.0 -lncurses -limf -liomp5 -lpthread

OBJS =  $(srcdir)/AZUREMain.o $(srcdir)/AChannel.o $(srcdir)/ALevel.o $(srcdir)/CNuc.o \
	$(srcdir)/Config.o $(srcdir)/DataLine.o $(srcdir)/Decay.o \
	$(srcdir)/EData.o $(srcdir)/EPoint.o $(srcdir)/ESegment.o \
	$(srcdir)/GSL_AngCoeff.o $(srcdir)/GSL_ShftFunc.o \
	$(srcdir)/GSL_ShftFunc_dE.o \
	$(srcdir)/Interference.o $(srcdir)/JGroup.o $(srcdir)/KGroup.o \
	$(srcdir)/KLGroup.o $(srcdir)/MGroup.o $(srcdir)/NucLine.o \
	$(srcdir)/PPair.o $(srcdir)/SegLine.o $(srcdir)/AZURECalc.o \
	$(srcdir)/GenMatrixFunc.o $(srcdir)/RMatrixFunc.o $(srcdir)/GSL_MatInv.o \
	$(srcdir)/AZUREOutput.o $(srcdir)/AMatrixFunc.o \
	$(srcdir)/ECLine.o $(srcdir)/ECMGroup.o $(srcdir)/ECIntegral.o \
	$(srcdir)/NFIntegral.o $(srcdir)/GSL_Diagonalize.o \
	$(srcdir)/GSL_PEshift_dE.o $(srcdir)/DoubleFactorial.o \
	$(srcdir)/ExtrapLine.o $(srcdir)/ReactionRate.o \
	$(srcdir)/ECLevel.o $(srcdir)/AZUREParams.o $(srcdir)/Equation.o \
	$(srcdir)/TargetEffect.o

AZURE2 : $(OBJS) $(srcdir)/AZURE2.cpp
	$(CXX) $(CPPFLAGS) -o $@ $(srcdir)/AZURE2.cpp $(OBJS) $(LIBS)

.PHONY : clean
clean :
	-rm $(OBJS)

.PHONY : install
install :
	-cp AZURE2 $(HOME)/bin

