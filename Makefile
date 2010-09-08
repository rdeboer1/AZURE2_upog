# Setup the prefixes below. Running 'make install' will copy file to $(PREFIX)/bin/AZURE2.

PREFIX = $(HOME)
MINUIT_PREFIX = $(PREFIX)
GSL_PREFIX = $(PREFIX)

# Change the variable below for the compiler.  Compiler should have 
#  dependency generation with -M.  Ok for intel and g++.

CXX = icc

# The following portion of the makefile should not need to be changed.

CPPFLAGS = -I../include -I$(MINUIT_PREFIX)/include
LFLAGS = -L$(MINUIT_PREFIX)/lib
ifneq ($(MINUIT_PREFIX),$(GSL_PREFIX))
   CPPFLAGS += -I$(GSL_PREFIX)/include
   LFLAGS += -L$(GSL_PREFIX)/lib
endif
LIBS = -lgsl -lgslcblas -lMinuit2 -lreadline
ifeq ($(CXX),icc)
   LIBS += -lglib-2.0 -lncurses -limf -liomp5 -lpthread
else 
   LIBS += -lgomp -lpthread
endif

srcdir = src

export CXX
export CPPFLAGS
export LFLAGS
export LIBS

all :   
	@(echo "Decending Into Source Directory....")
	@(cd $(srcdir); $(MAKE))

.PHONY : clean
clean : 
	@(echo "Cleaning Up...")
	@(cd $(srcdir); $(MAKE) clean)

.PHONY : install
install : 
	cp AZURE2 $(PREFIX)/bin/
