LIBRASCH_HOME = ../..
include $(LIBRASCH_HOME)/Makefile.incl

all: ra_matlab.c ra_matlab.h
	$(Q)mex -g ra_matlab.c -I$(LIBRASCH_HOME)/include -L$(LIBRASCH_HOME) -lrasch
	$(Q)cp ra_matlab.mexglx ./m_files/ra_matlab.mexglx

clean:
	@echo $(INDENT)clean Matlab interface
	$(Q)rm -rf ra_matlab.mexglx m_files/ra_matlab.mexglx

uninstall:
