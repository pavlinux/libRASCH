LIBRASCH_HOME = ../../..
include $(LIBRASCH_HOME)/Makefile.incl

OBJS = dasy.o

all: $(OBJS)
	@echo $(INDENT)LD $(RELDIR)/dasylab.o
	$(Q)$(LD) $(LDFLAGS_PL) -o dasylab.o dasy.o $(LIBRASCH_HOME)/src/shared/pl_general.o $(LIBRASCH_HOME)/src/shared/ra_file.o
	$(Q)cp dasylab.o $(LIBRASCH_HOME)/plugins/bin/dasylab.o

dasy.o: dasylab.c dasylab.h $(LIBRASCH_HOME)/include/handle_plugin.h $(LIBRASCH_HOME)/include/ra_endian.h $(LIBRASCH_HOME)/include/ra_file.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))	
	$(Q)$(CC) $(CFLAGS) -o dasy.o dasylab.c

clean:
	@echo $(INDENT)clean plugin dasylab
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/dasylab.o
	$(Q)rm -f *.o

# $Header: /home/cvs_repos.bak/librasch/plugins/access/dasylab/Makefile,v 1.5 2004/07/13 14:11:08 rasch Exp $
