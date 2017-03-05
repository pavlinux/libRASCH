LIBRASCH_HOME = ../../..
include $(LIBRASCH_HOME)/Makefile.incl

OBJS = respiration.o
LDFLAGS_PL_USE = $(LDFLAGS_PL) -lfftw3 -lm

all: $(OBJS)
	@echo $(INDENT)LD $(RELDIR)/respiration.o
	$(Q)$(LD) $(LDFLAGS_PL_USE) -o resp.o ${OBJS}
	$(Q)cp resp.o $(LIBRASCH_HOME)/plugins/bin/respiration.o

respiration.o: respiration.c respiration.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q)$(CC) $(CFLAGS) -o respiration.o respiration.c

clean:
	@echo $(INDENT)clean plugin respiration
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/respiration.o
	$(Q)rm -f *.o

# $Id$
