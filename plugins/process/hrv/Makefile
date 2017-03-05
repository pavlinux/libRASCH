LIBRASCH_HOME = ../../..
include $(LIBRASCH_HOME)/Makefile.incl

OBJS = hrv.o stat.o

all: $(OBJS)
	@echo $(INDENT)LD $(RELDIR)/hrv.o
	$(Q)$(LD) $(LDFLAGS_PL) -o hrv_t.o $(OBJS)
	$(Q)cp hrv_t.o $(LIBRASCH_HOME)/plugins/bin/hrv.o

hrv.o: hrv.c hrv.h frequency.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q)$(CC) $(CFLAGS) -o hrv.o hrv.c

stat.o: statistics.c statistics.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q)$(CC) $(CFLAGS) -o stat.o statistics.c

clean:
	@echo $(INDENT)clean plugin hrv
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/hrv.o
	$(Q)rm -f *.o

# $Header: /home/cvs_repos.bak/librasch/plugins/process/hrv/Makefile,v 1.6 2004/07/13 14:11:08 rasch Exp $
