LIBRASCH_HOME = ../../..
include $(LIBRASCH_HOME)/Makefile.incl

OBJS = cfs_t.o

all: $(OBJS)
	@echo $(INDENT)LD $(RELDIR)/cfs.o
	$(Q)$(LD) $(LDFLAGS_PL) -o cfs.o $(OBJS) $(LIBRASCH_HOME)/src/shared/pl_general.o $(LIBRASCH_HOME)/src/shared/ra_file.o
	$(Q)cp cfs.o $(LIBRASCH_HOME)/plugins/bin/cfs.o

cfs_t.o: cfs.c cfs.h $(LIBRASCH_HOME)/include/handle_plugin.h $(LIBRASCH_HOME)/include/ra_endian.h $(LIBRASCH_HOME)/include/ra_file.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))	
	$(Q)$(CC) $(CFLAGS) -o cfs_t.o cfs.c

clean:
	@echo $(INDENT)clean plugin cfs
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/cfs.o
	$(Q)rm -f *.o

# $Header: /home/cvs_repos.bak/librasch/plugins/access/cfs/Makefile,v 1.5 2004/07/13 14:11:08 rasch Exp $
