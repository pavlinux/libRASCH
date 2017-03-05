LIBRASCH_HOME = ../../..
include $(LIBRASCH_HOME)/Makefile.incl

OBJS = ecg.o

all: $(OBJS)
	@echo $(INDENT)LD $(RELDIR)/ecg.o
	$(Q)$(LD) $(LDFLAGS_PL) -o ecg_t.o ${OBJS}
	$(Q)cp ecg_t.o $(LIBRASCH_HOME)/plugins/bin/ecg.o

ecg.o: ecg.c ecg.h $(LIBRASCH_HOME)/include/ra_ecg.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q)$(CC) $(CFLAGS) -o ecg.o ecg.c

clean:
	@echo $(INDENT)clean plugin ecg
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/ecg.o
	$(Q)rm -f *.o

# $Header: /home/cvs_repos.bak/librasch/plugins/process/ecg/Makefile,v 1.5 2004/07/13 14:11:08 rasch Exp $
