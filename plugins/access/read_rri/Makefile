LIBRASCH_HOME = ../../..
include $(LIBRASCH_HOME)/Makefile.incl

OBJS = read_rri_t.o

all: $(OBJS)
	@echo $(INDENT)LD $(RELDIR)/read_rri.o
	$(Q)$(LD) $(LDFLAGS_PL) -o read_rri.o $(LIBRASCH_HOME)/src/shared/pl_general.o $(LIBRASCH_HOME)/src/shared/ra_file.o ${OBJS}
	$(Q)cp read_rri.o $(LIBRASCH_HOME)/plugins/bin/read_rri.o

read_rri_t.o: read_rri.c read_rri.h $(LIBRASCH_HOME)/include/handle_plugin.h $(LIBRASCH_HOME)/include/ra_file.h occ.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))	
	$(Q)$(CC) $(CFLAGS) -o read_rri_t.o read_rri.c

clean:
	@echo $(INDENT)clean plugin read-rri
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/read_rri.o
	$(Q)rm -f *.o

# $Id$
