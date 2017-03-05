LIBRASCH_HOME = ../../..
include $(LIBRASCH_HOME)/Makefile.incl

OBJS = porti.o

all: $(OBJS)
	@echo $(INDENT)LD $(RELDIR)/portilab.o
	$(Q)$(LD) $(LDFLAGS_PL) -o portilab.o porti.o $(LIBRASCH_HOME)/src/shared/pl_general.o $(LIBRASCH_HOME)/src/shared/ra_file.o
	$(Q)cp portilab.o $(LIBRASCH_HOME)/plugins/bin/portilab.o

porti.o: portilab.c portilab.h $(LIBRASCH_HOME)/include/handle_plugin.h $(LIBRASCH_HOME)/include/ra_endian.h $(LIBRASCH_HOME)/include/ra_file.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))	
	$(Q)$(CC) $(CFLAGS) -o porti.o portilab.c

clean:
	@echo $(INDENT)clean plugin portilab
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/portilab.o
	$(Q)rm -f *.o

# $Header: /home/cvs_repos.bak/librasch/plugins/access/portilab/Makefile,v 1.5 2004/07/13 14:11:08 rasch Exp $
