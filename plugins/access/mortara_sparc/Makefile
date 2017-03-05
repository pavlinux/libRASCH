LIBRASCH_HOME = ../../..
include $(LIBRASCH_HOME)/Makefile.incl

OBJS = mortara_sparc_t.o

all: $(OBJS)
	@echo $(INDENT)LD $(RELDIR)/mortara_sparc.o
	$(Q)$(LD) $(LDFLAGS_PL) -o mortara_sparc.o $(LIBRASCH_HOME)/src/shared/pl_general.o\
		$(LIBRASCH_HOME)/src/shared/ra_file.o ${OBJS}
	$(Q)cp mortara_sparc.o $(LIBRASCH_HOME)/plugins/bin/mortara_sparc.o

mortara_sparc_t.o: mortara_sparc.c mortara_sparc.h $(LIBRASCH_HOME)/include/handle_plugin.h\
	 $(LIBRASCH_HOME)/include/ra_endian.h $(LIBRASCH_HOME)/include/ra_file.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))	
	$(Q)$(CC) $(CFLAGS) -o mortara_sparc_t.o mortara_sparc.c

clean:
	@echo $(INDENT)clean plugin Mortara-SPARC
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/mortara_sparc.o
	$(Q)rm -f *.o

# $Id$
