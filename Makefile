# Setup the prefixes below. Running 'make install' will copy file to $(PREFIX)/bin/AZURE2.

PREFIX = $(HOME)/local
MINUIT_PREFIX = $(PREFIX)
GSL_PREFIX = $(PREFIX)

# Change the variables below for the compiler.  Compiler should have 
#  dependency generation with -M, and macro setting with -D.  Ok for icc and g++.

CXX = icpc
AR = ar
RANLIB = ranlib
LD = icpc

# Change the following variable to 'no' if the system libraries do not include stat() command.
#  This will turn off checking that input/output files/directories exist at start, 
#  so you'd better be careful with your paths!

USE_STAT=yes

# Change this variable to specify certain flags needed for linking on Windows systems (at least under MSYS).

WINDOWS=no

# Used for cross compiling.  The average user shouldn't need these variables.

CROSS=no
CROSS_HOST=i686-pc-mingw32-

# The following portion of the makefile should not need to be changed if the g++ or icc compiler
#  is used.  Otherwise, no promises...

ifeq ($(CROSS),yes) 
   CXX:=$(CROSS_HOST)$(CXX)
   AR:=$(CROSS_HOST)$(AR)
   RANLIB:=$(CROSS_HOST)$(RANLIB)
   LD:=$(CROSS_HOST)$(LD)
endif

CPPFLAGS = -I../include -I$(MINUIT_PREFIX)/include
ifneq ($(USE_STAT),yes) 
   CPPFLAGS += -DNO_STAT
endif
LFLAGS = -L$(MINUIT_PREFIX)/lib
ifneq ($(WINDOWS),no)
   LFLAGS += -Wl,--enable-auto-import
endif
ifneq ($(MINUIT_PREFIX),$(GSL_PREFIX))
   CPPFLAGS += -I$(GSL_PREFIX)/include
   LFLAGS += -L$(GSL_PREFIX)/lib
endif
LIBS = -lgsl -lgslcblas -lMinuit2 -lreadline 
ifeq ($(CXX),icpc)
   CPPFLAGS += -openmp
   LFLAGS += -openmp
   LIBS += -lglib-2.0 -lncurses -limf -liomp5 -lpthread
else 
   CPPFLAGS += -fopenmp
   LFLAGS += -fopenmp
   LIBS += -lgomp -lpthread
endif
ifeq ($(CROSS),yes)
   LIBS += -lpdcurses
endif

srcdir = src
coul_srcdir = coul/src

export CXX
export AR
export RANLIB
export LD

all :   
	@(rm -f AZURE2)
	@(echo "Decending Into Source Directory....")
	@(cd $(srcdir); $(MAKE) CPPFLAGS="$(CPPFLAGS)" LIBS="$(LIBS)" LFLAGS="$(LFLAGS)")

accurate : 
	@(rm -f AZURE2)
	@(echo "Decending Coulomb Library Directory....")
	@(cd $(coul_srcdir); $(MAKE))
	@(echo "Decending Into Source Directory....")
	@(cd $(srcdir); $(MAKE) CPPFLAGS="$(CPPFLAGS) -I../coul/include -DEXT_COUL" LIBS="$(LIBS) -lcoul" LFLAGS="$(LFLAGS) -L../lib")

setup :
	@(rm -f AZURE2)
	@(echo "Building AZURESetup2...")
	@(cd gui; qmake -t app -o Makefile.app; qmake; make;)
	@(echo "Decending Into Source Directory....")
	$(eval QT_LIBS := `egrep 'LIBS[[:blank:]]+=' ../gui/Makefile.app | sed 's/LIBS[[:blank:]]\+=//'`)
	$(eval QT_LFLAGS := `egrep 'LFLAGS[[:blank:]]+=' ../gui/Makefile.app | sed 's/LFLAGS[[:blank:]]\+=//'`)
	@(cd $(srcdir); $(MAKE) CPPFLAGS="-DGUI_BUILD $(CPPFLAGS)" LIBS="-L../gui -lAZURESetup2 $(LIBS) $(QT_LIBS)" LFLAGS="$(QT_LFLAGS) $(LFLAGS)")

setup-accurate :
	@(rm -f AZURE2)
	@(echo "Decending Coulomb Library Directory....")
	@(cd $(coul_srcdir); $(MAKE))
	@(echo "Building AZURESetup2...")
	@(cd gui; qmake -t app -o Makefile.app; qmake; make;)
	@(echo "Decending Into Source Directory....")
	$(eval QT_LIBS := `egrep 'LIBS[[:blank:]]+=' ../gui/Makefile.app | sed 's/LIBS[[:blank:]]\+=//'`)
	$(eval QT_LFLAGS := `egrep 'LFLAGS[[:blank:]]+=' ../gui/Makefile.app | sed 's/LFLAGS[[:blank:]]\+=//'`)
	@(cd $(srcdir); $(MAKE) CPPFLAGS="-DGUI_BUILD -I../coul/include -DEXT_COUL $(CPPFLAGS)" LIBS="-L../gui -lAZURESetup2 -L../lib -lcoul $(LIBS) $(QT_LIBS)" LFLAGS="$(QT_LFLAGS) $(LFLAGS)")


.PHONY : clean
clean : 
	@(echo "Cleaning Up In Coulomb Library Directory...")
	@(cd $(coul_srcdir); $(MAKE) clean)
	@(echo "Cleaning Up AZURESetup2...")
	@(cd gui; make clean; rm -f Makefile; rm -f Makefile.app; rm -f libAZURESetup2.a)
	@(echo "Cleaning Up In Source Directory...")
	@(cd $(srcdir); $(MAKE) clean)
	-rm -rf lib

.PHONY : install
install : 
	cp AZURE2 $(PREFIX)/bin/
