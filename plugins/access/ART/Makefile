LIBRASCH_HOME = ../../..
include $(LIBRASCH_HOME)/Makefile.incl

OBJS = ART_t.o

all: $(OBJS)
	@echo $(INDENT)LD $(RELDIR)/ART.o
	$(Q)$(LD) $(LDFLAGS_PL) -o ART.o ART_t.o $(LIBRASCH_HOME)/src/shared/pl_general.o $(LIBRASCH_HOME)/src/shared/ra_file.o
	$(Q)cp ART.o $(LIBRASCH_HOME)/plugins/bin/ART.o

ART_t.o: ART.c ART.h $(LIBRASCH_HOME)/include/handle_plugin.h $(LIBRASCH_HOME)/include/ra_endian.h $(LIBRASCH_HOME)/include/ra_file.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))	
	$(Q)$(CC) $(CFLAGS) -o ART_t.o ART.c

clean:
	@echo $(INDENT)clean plugin ART
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/ART.o
	$(Q)rm -f *.o

# $Header: /home/cvs_repos.bak/librasch/plugins/access/ART/Makefile,v 1.5 2004/07/13 14:11:08 rasch Exp $
