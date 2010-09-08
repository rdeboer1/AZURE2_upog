# Running 'make install' will copy file to $(PREFIX)/bin/AZURE2

PREFIX = $(HOME)

# Change the variable below for the compiler.  Compiler should have 
#  dependency generation with -M.  Ok for intel and g++.

CXX = icc

# Uncomment the following line if Minuit2 and Root are installed
#  locally.

CPPFLAGS =  -I$(HOME)/include 
LFLAGS = -L$(HOME)/lib

# Change the following lines for system dependent libraries.  For instace 
# the OpenMP implimentation might change between compilers

#LIBS = -lgomp #This line is needed for g++
LIBS  = -lglib-2.0 -lncurses -limf -liomp5  #This line is needed for CRC environment.

# The following portion of the makefile should not need to be changed.

CPPFLAGS += -I../include 
LFLAGS += -L../lib
LIBS += -lgsl -lgslcblas -lMinuit2 -lcoul -lreadline -lpthread

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
