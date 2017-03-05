LIBRASCH_HOME = ../../..
include $(LIBRASCH_HOME)/Makefile.incl

OBJS = ishne_t.o

all: $(OBJS)
	@echo $(INDENT)LD $(RELDIR)/ishne.o
	$(Q)$(LD) $(LDFLAGS_PL) -o ishne.o $(LIBRASCH_HOME)/src/shared/pl_general.o $(LIBRASCH_HOME)/src/shared/ra_file.o ${OBJS}
	$(Q)cp ishne.o $(LIBRASCH_HOME)/plugins/bin/ishne.o

ishne_t.o: ishne.c ishne.h $(LIBRASCH_HOME)/include/handle_plugin.h $(LIBRASCH_HOME)/include/ra_endian.h $(LIBRASCH_HOME)/include/ra_file.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))	
	$(Q)$(CC) $(CFLAGS) -o ishne_t.o ishne.c

clean:
	@echo $(INDENT)clean plugin ISHNE
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/ishne.o
	$(Q)rm -f *.o

# $Header: /home/cvs_repos.bak/librasch/plugins/access/ishne/Makefile,v 1.5 2004/07/13 14:11:08 rasch Exp $
