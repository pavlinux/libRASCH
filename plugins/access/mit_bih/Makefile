LIBRASCH_HOME = ../../..
include $(LIBRASCH_HOME)/Makefile.incl

OBJS = mit_bih_t.o

all: $(OBJS)
	@echo $(INDENT)LD $(RELDIR)/mit_bih.o
	$(Q)$(LD) $(LDFLAGS_PL) -o mit_bih.o ${OBJS} $(LIBRASCH_HOME)/src/shared/pl_general.o $(LIBRASCH_HOME)/src/shared/ra_file.o
	$(Q)cp mit_bih.o $(LIBRASCH_HOME)/plugins/bin/mit_bih.o

mit_bih_t.o: mit_bih.c mit_bih.h $(LIBRASCH_HOME)/include/handle_plugin.h $(LIBRASCH_HOME)/include/ra_endian.h $(LIBRASCH_HOME)/include/ra_file.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))	
	$(Q)$(CC) $(CFLAGS) -o mit_bih_t.o mit_bih.c

clean:
	@echo $(INDENT)clean plugin MIT/BIH
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/mit_bih.o
	$(Q)rm -f *.o

# $Header: /home/cvs_repos.bak/librasch/plugins/access/mit_bih/Makefile,v 1.7 2004/11/03 14:37:04 rasch Exp $
