COUL_depsdir=.deps

MAKEDEPEND = $(CXX) -I../include -M $< | sed 's/$@/& $(COUL_depsdir)\/$*.d/g' > $(COUL_depsdir)/$*.d

COUL_SRCS=complex_functions.cpp\
	  cwfcomp.cpp\
	  ode_int.cpp

../../lib/libcoul.a : $(COUL_SRCS:.cpp=.o)
	ar cr $@ $(COUL_SRCS:.cpp=.o)

%.o : %.cpp | $(COUL_depsdir)
	@$(MAKEDEPEND)
	$(CXX)  -I../include -c -o $@ $<

-include $(COUL_SRCS:%.cpp=$(COUL_depsdir)/%.d)

$(COUL_depsdir) :
	@(mkdir $(COUL_depsdir))

.PHONY : clean
clean :
	-rm -f $(COUL_SRCS:.cpp=.o)
	-rm -rf $(COUL_depsdir)
