LIBRASCH_HOME = ../../..
include $(LIBRASCH_HOME)/Makefile.incl


OBJS = ctg_sonicaidt.o

all: $(OBJS)
	@echo $(INDENT)LD $(RELDIR)/ctg_sonicaid.o
	$(Q)$(LD) $(LDFLAGS_PL) -o ctg_sonicaid.o ctg_sonicaidt.o $(LIBRASCH_HOME)/src/shared/pl_general.o $(LIBRASCH_HOME)/src/shared/ra_file.o
	$(Q)cp ctg_sonicaid.o $(LIBRASCH_HOME)/plugins/bin/ctg_sonicaid.o

ctg_sonicaidt.o: ctg_sonicaid.c ctg_sonicaid.h $(LIBRASCH_HOME)/include/handle_plugin.h $(LIBRASCH_HOME)/include/ra_endian.h $(LIBRASCH_HOME)/include/ra_file.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))	
	$(Q)$(CC) $(CFLAGS) -o ctg_sonicaidt.o ctg_sonicaid.c

clean:
	@echo $(INDENT)clean plugin ctg-sonicaid
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/ctg_sonicaid.o
	$(Q)rm -f *.o

# $Id$
