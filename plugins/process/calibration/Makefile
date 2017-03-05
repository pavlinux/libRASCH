LIBRASCH_HOME = ../../..
include $(LIBRASCH_HOME)/Makefile.incl

OBJS = calib.o
LDFLAGS_USE = $(LDFLAGS_PL) -lm

all: $(OBJS)
	@echo $(INDENT)LD $(RELDIR)/calibration.o
	$(Q)$(LD) $(LDFLAGS_USE) -o calibration.o ${OBJS} $(LIBRASCH_HOME)/src/shared/pl_general.o
	$(Q)cp calibration.o $(LIBRASCH_HOME)/plugins/bin/calibration.o

calib.o: calibration.c calibration.h $(LIBRASCH_HOME)/include/handle_plugin.h $(LIBRASCH_HOME)/include/ra.h $(LIBRASCH_HOME)/include/ra_priv.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q)$(CC) $(CFLAGS) -o calib.o calibration.c

clean:
	@echo $(INDENT)clean plugin calibration
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/calibration.o
	$(Q)rm -f *.o

# $Id$
