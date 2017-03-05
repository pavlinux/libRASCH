LIBRASCH_HOME = ../../..
include $(LIBRASCH_HOME)/Makefile.incl

ifeq "$(PL_USE_FFTW3)" "yes"
CFLAGS_USE = $(CFLAGS) -DPL_USE_FFTW3
LDFLAGS_PL_USE = $(LDFLAGS_PL) -lfftw3 -lm
else
CFLAGS_USE = $(CFLAGS)
LDFLAGS_PL_USE = $(LDFLAGS_PL) -lm
endif

OBJS = freq_analysis.o frequency.o stat.o


all: $(OBJS)
	@echo $(INDENT)LD $(RELDIR)/freq_analysis.o
	$(Q)$(LD) $(LDFLAGS_PL_USE) -o freq_analysis_t.o $(OBJS)
ifeq "$(PL_USE_FFTW3)" "yes"
	$(Q)cp freq_analysis_t.o $(LIBRASCH_HOME)/plugins/bin/freq_analysis.o
else
	$(Q)cp freq_analysis_t.o $(LIBRASCH_HOME)/plugins/bin/freq_analysis_non_fftw3.o
endif

freq_analysis.o: freq_analysis.c freq_analysis.h frequency_fftw3.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q)$(CC) $(CFLAGS_USE) -o freq_analysis.o freq_analysis.c

stat.o: statistics.c statistics.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q)$(CC) $(CFLAGS_USE) -o stat.o statistics.c

frequency.o: frequency_fftw3.c frequency_fftw3.h fft.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q)$(CC) $(CFLAGS_USE) -DPL_USE_FFTW3 -o frequency.o frequency_fftw3.c

clean:
	@echo $(INDENT)clean plugin freq_analysis
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/freq_analysis.o
	$(Q)rm -f *.o

