LIBRASCH_HOME = ../../..
include $(LIBRASCH_HOME)/Makefile.incl

OBJS = dadisp_t.o

all: $(OBJS)
	@echo $(INDENT)LD $(RELDIR)/dadisp.o
	$(Q)$(LD) $(LDFLAGS_PL) -o dadisp.o ${OBJS} $(LIBRASCH_HOME)/src/shared/pl_general.o $(LIBRASCH_HOME)/src/shared/ra_file.o
	$(Q)cp dadisp.o $(LIBRASCH_HOME)/plugins/bin/dadisp.o

dadisp_t.o: dadisp.c dadisp.h $(LIBRASCH_HOME)/include/handle_plugin.h $(LIBRASCH_HOME)/include/ra_endian.h $(LIBRASCH_HOME)/include/ra_file.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))	
	$(Q)$(CC) $(CFLAGS) -o dadisp_t.o dadisp.c

clean:
	@echo $(INDENT)clean plugin DaDisp
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/dadisp.o
	$(Q)rm -f *.o

# $Id$
