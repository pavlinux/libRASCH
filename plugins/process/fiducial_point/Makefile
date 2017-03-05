LIBRASCH_HOME = ../../..
include $(LIBRASCH_HOME)/Makefile.incl

OBJS = fid.o

all: $(OBJS)
	@echo $(INDENT)LD $(RELDIR)/fiducial_point.o
	$(Q)$(LD) $(LDFLAGS_PL) -o fiducial_point.o ${OBJS} $(LIBRASCH_HOME)/src/shared/pl_general.o
	$(Q)cp fiducial_point.o $(LIBRASCH_HOME)/plugins/bin/fiducial_point.o

fid.o: fiducial.c fiducial.h $(LIBRASCH_HOME)/include/handle_plugin.h $(LIBRASCH_HOME)/include/ra.h $(LIBRASCH_HOME)/include/ra_defines.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q)$(CC) $(CFLAGS) -o fid.o fiducial.c

clean:
	@echo $(INDENT)clean plugin fiducial_point
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/fiducial_point.o
	$(Q)rm -f *.o

# $Id$
