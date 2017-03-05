LIBRASCH_HOME = ../../..
include $(LIBRASCH_HOME)/Makefile.incl

OBJS = edf_t.o

all: $(OBJS)
	@echo $(INDENT)LD $(RELDIR)/edf.o
	$(Q)$(LD) $(LDFLAGS_PL) -o edf.o ${OBJS} $(LIBRASCH_HOME)/src/shared/pl_general.o $(LIBRASCH_HOME)/src/shared/ra_file.o
	$(Q)cp edf.o $(LIBRASCH_HOME)/plugins/bin/edf.o

edf_t.o: edf.c edf.h $(LIBRASCH_HOME)/include/handle_plugin.h $(LIBRASCH_HOME)/include/ra_endian.h $(LIBRASCH_HOME)/include/ra_file.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))	
	$(Q)$(CC) $(CFLAGS) -o edf_t.o edf.c

clean:
	@echo $(INDENT)clean plugin EDF
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/edf.o
	$(Q)rm -f *.o

# $Header: /home/cvs_repos.bak/librasch/plugins/access/edf/Makefile,v 1.6 2004/07/25 20:29:38 rasch Exp $
