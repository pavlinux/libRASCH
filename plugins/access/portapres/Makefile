LIBRASCH_HOME = ../../..
include $(LIBRASCH_HOME)/Makefile.incl

OBJS = porta.o

all: $(OBJS)
	@echo $(INDENT)LD $(RELDIR)/portapres.o
	$(Q)$(LD) $(LDFLAGS_PL) -o portapres.o porta.o $(LIBRASCH_HOME)/src/shared/pl_general.o $(LIBRASCH_HOME)/src/shared/ra_file.o
	$(Q)cp portapres.o $(LIBRASCH_HOME)/plugins/bin/portapres.o

porta.o: portapres.c portapres.h $(LIBRASCH_HOME)/include/handle_plugin.h $(LIBRASCH_HOME)/include/ra_endian.h $(LIBRASCH_HOME)/include/ra_file.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))	
	$(Q)$(CC) $(CFLAGS) -o porta.o portapres.c

clean:
	@echo $(INDENT)clean plugin portapres
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/portapres.o
	$(Q)rm -f *.o

# $Header: /home/cvs_repos.bak/librasch/plugins/access/portapres/Makefile,v 1.5 2004/07/13 14:11:08 rasch Exp $
