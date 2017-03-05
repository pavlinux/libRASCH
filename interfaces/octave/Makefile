LIBRASCH_HOME = ../..
include ../../Makefile.incl

all: ra_octave.cc
	$(Q)mkoctfile ra_octave.cc -I$(LIBRASCH_HOME)/include -I$(OCTAVE_INCL_PATH) -L$(LIBRASCH_HOME) -lrasch
	$(Q)cp ra_octave.oct m_files/

clean:
	@echo $(INDENT)clean Octave interface
	$(Q)rm -rf ra_octave.oct m_files/ra_octave.oct

install:
	install -d $(OCTAVE_FILE_DIR)/librasch
	install --mode=644 m_files/*.m $(OCTAVE_FILE_DIR)/librasch
	install --mode=644 m_files/*.oct $(OCTAVE_FILE_DIR)/librasch

uninstall:
	rm -rf $(OCTAVE_FILE_DIR)/librasch;



