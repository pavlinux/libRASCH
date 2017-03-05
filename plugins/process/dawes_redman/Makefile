LIBRASCH_HOME = ../../..
include $(LIBRASCH_HOME)/Makefile.incl

OBJS = dawes_redman.o

all: $(OBJS)
	@echo $(INDENT)LD $(RELDIR)/dawes_redman.o
	$(Q)$(LD) $(LDFLAGS_PL) -o dr.o ${OBJS}
	$(Q)cp dr.o $(LIBRASCH_HOME)/plugins/bin/dawes_redman.o

dawes_redman.o: dawes_redman.c dawes_redman.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q)$(CC) $(CFLAGS) -o dawes_redman.o dawes_redman.c

clean:
	@echo $(INDENT)clean plugin dawes_redman
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/dawes_redman.o
	$(Q)rm -f *.o

# $Header: /home/cvs_repos.bak/librasch/plugins/process/dawes_redman/Makefile,v 1.3 2004/07/13 14:11:08 rasch Exp $
