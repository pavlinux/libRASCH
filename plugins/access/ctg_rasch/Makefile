LIBRASCH_HOME = ../../..
include $(LIBRASCH_HOME)/Makefile.incl


OBJS = ctg.o

all: $(OBJS)
	@echo $(INDENT)LD $(RELDIR)/ctg_rasch.o
	$(Q)$(LD) $(LDFLAGS_PL) -o ctg_rasch.o ctg.o $(LIBRASCH_HOME)/src/shared/pl_general.o $(LIBRASCH_HOME)/src/shared/ra_file.o
	$(Q)cp ctg_rasch.o $(LIBRASCH_HOME)/plugins/bin/ctg_rasch.o

ctg.o: ctg.c ctg.h $(LIBRASCH_HOME)/include/handle_plugin.h $(LIBRASCH_HOME)/include/ra_endian.h $(LIBRASCH_HOME)/include/ra_file.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))	
	$(Q)$(CC) $(CFLAGS) -o ctg.o ctg.c

clean:
	@echo $(INDENT)clean plugin ctg-rasch
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/ctg_rasch.o
	$(Q)rm -f *.o

# $Header: /home/cvs_repos.bak/librasch/plugins/access/ctg_rasch/Makefile,v 1.4 2004/07/13 14:11:08 rasch Exp $
