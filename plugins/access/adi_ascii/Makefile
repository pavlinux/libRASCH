LIBRASCH_HOME = ../../..
include $(LIBRASCH_HOME)/Makefile.incl

OBJS = adi_ascii_t.o

all: $(OBJS)
	@echo $(INDENT)LD $(RELDIR)/adi_ascii.o
	$(Q)$(LD) $(LDFLAGS_PL) -o adi_ascii.o $(LIBRASCH_HOME)/src/shared/pl_general.o $(LIBRASCH_HOME)/src/shared/ra_file.o ${OBJS}
	$(Q)cp adi_ascii.o $(LIBRASCH_HOME)/plugins/bin/adi_ascii.o

adi_ascii_t.o: adi_ascii.c adi_ascii.h $(LIBRASCH_HOME)/include/handle_plugin.h $(LIBRASCH_HOME)/include/ra_endian.h $(LIBRASCH_HOME)/include/ra_file.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))	
	$(Q)$(CC) $(CFLAGS) -o adi_ascii_t.o adi_ascii.c

clean:
	@echo $(INDENT)clean plugin ADI-ASCII
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/adi_ascii.o
	$(Q)rm -f *.o

# $Id$
