# Setup the prefixes below. Running 'make install' will copy file to $(PREFIX)/bin/AZURE2.

PREFIX = $(HOME)
MINUIT_PREFIX = $(PREFIX)
GSL_PREFIX = $(PREFIX)

# Change the variable below for the compiler.  Compiler should have 
#  dependency generation with -M, and macro setting with -D.  Ok for icc and g++.

CXX = icpc

# Change the following variable to 'no' if the system libraries do not include stat() command.
#  This will turn off checking that input/output files/directories exist at start, 
#  so you'd better be careful with your paths!

USE_STAT=yes

# The following portion of the makefile should not need to be changed if the g++ or icc compiler
#  is used.  Otherwise, no promises...

CPPFLAGS = -I../include -I$(MINUIT_PREFIX)/include
ifneq ($(USE_STAT),yes) 
   CPPFLAGS += -DNO_STAT
endif
LFLAGS = -L$(MINUIT_PREFIX)/lib
ifneq ($(MINUIT_PREFIX),$(GSL_PREFIX))
   CPPFLAGS += -I$(GSL_PREFIX)/include
   LFLAGS += -L$(GSL_PREFIX)/lib
endif
LIBS = -lgsl -lgslcblas -lMinuit2 -lreadline
ifeq ($(CXX),icpc)
   CPPFLAGS += -openmp
   LIBS += -lglib-2.0 -lncurses -limf -liomp5 -lpthread
else 
   CPPFLAGS += -fopenmp
   LIBS += -lgomp -lpthread
endif

srcdir = src
coul_srcdir = coul/src

export CXX

all :   
	@(echo "Decending Into Source Directory....")
	@(cd $(srcdir); $(MAKE) CPPFLAGS="$(CPPFLAGS)" LIBS="$(LIBS)" LFLAGS="$(LFLAGS)")

accurate : 
	@(echo "Decending Coulomb Library Directory....")
	@(cd $(coul_srcdir); $(MAKE))
	@(echo "Decending Into Source Directory....")
	@(cd $(srcdir); $(MAKE) CPPFLAGS="$(CPPFLAGS) -I../coul/include -DEXT_COUL" LIBS="$(LIBS) -lcoul" LFLAGS="$(LFLAGS) -L../lib")


.PHONY : clean
clean : 
	@(echo "Cleaning Up In Coulomb Library Directory...")
	@(cd $(coul_srcdir); $(MAKE) clean)
	@(echo "Cleaning Up In Source Directory...")
	@(cd $(srcdir); $(MAKE) clean)
	-rm -rf lib

.PHONY : install
install : 
	cp AZURE2 $(PREFIX)/bin/
