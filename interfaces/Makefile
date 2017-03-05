########## configure specific settings ##########
PERL_INSTALLED = yes
PYTHON_INSTALLED = yes
OCTAVE_INSTALLED = yes
MATLAB_INSTALLED = yes
SCILAB_INSTALLED = 

########## Makefile.in
INDENT = '  '

all: cpp perl python matlab octave scilab

cpp: FORCE
	@cd c++ && $(MAKE) RELDIR=$(RELDIR)/c++

perl: interface.ok FORCE
ifneq "$(PERL_INSTALLED)" ""
	@echo $(INDENT)build Perl interface
	@cd perl && perl Makefile.PL > build_makefile.out && $(MAKE) > build.out
endif


python: interface.ok FORCE
ifneq "$(PYTHON_INSTALLED)" ""
	@echo $(INDENT)build Python interface
	@cd python && python setup.py build > build.out
endif


matlab: interface.ok FORCE
ifneq "$(MATLAB_INSTALLED)" ""
	@echo $(INDENT)build Matlab interface
	@cd matlab && $(MAKE) RELDIR=$(RELDIR)/matlab
endif


octave: interface.ok FORCE
ifneq "$(OCTAVE_INSTALLED)" ""
	@echo $(INDENT)build Octave interface
	@cd octave && $(MAKE) RELDIR=$(RELDIR)/octave
endif


scilab: interface.ok FORCE
ifneq "$(SCILAB_INSTALLED)" ""
	@echo $(INDENT)build Scilab interface
	@cd scilab && $(MAKE) RELDIR=$(RELDIR)/scilab
endif

FORCE:

interface.ok: make_interfaces.pl shared/interface_desc.xml shared/infos.h
	@echo "dummy" > interface.ok
	@perl make_interfaces.pl

install:
	@cd c++ && $(MAKE) install
ifneq "$(PERL_INSTALLED)" ""
	@cd perl && $(MAKE) install
endif
ifneq "$(PYTHON_INSTALLED)" ""
	@cd python && python setup.py install
endif
ifneq "$(OCTAVE_INSTALLED)" ""
	@cd octave && $(MAKE) install
endif


uninstall:
	@cd c++ && $(MAKE) uninstall
#ifneq "$(PERL_INSTALLED)" ""
#	@cd perl && $(MAKE) uninstall
#endif
#ifneq "$(PYTHON_INSTALLED)" ""
#	@cd python && python setup.py uninstall
#endif
ifneq "$(OCTAVE_INSTALLED)" ""
	@cd octave && $(MAKE) uninstall
endif


clean:
	- @cd c++ && $(MAKE) clean
	- @cd perl && $(MAKE) clean
	- @cd python && python setup.py clean
	- @cd matlab && $(MAKE) clean
	- @cd octave && $(MAKE) clean
	- @cd scilab && $(MAKE) clean

distclean: clean
