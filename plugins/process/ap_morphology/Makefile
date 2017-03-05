LIBRASCH_HOME = ../../..
include $(LIBRASCH_HOME)/Makefile.incl

OBJS = morph.o

all: $(OBJS)
	@echo $(INDENT)LD $(RELDIR)/ap_morphology.o
	$(Q)$(LD) $(LDFLAGS_PL) -o ap_morphology.o ${OBJS} $(LIBRASCH_HOME)/src/shared/pl_general.o
	$(Q)cp ap_morphology.o $(LIBRASCH_HOME)/plugins/bin/ap_morphology.o

morph.o: ap_morphology.c ap_morphology.h $(LIBRASCH_HOME)/include/handle_plugin.h $(LIBRASCH_HOME)/include/ra.h $(LIBRASCH_HOME)/include/ra_defines.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))	
	$(Q)$(CC) $(CFLAGS) -o morph.o ap_morphology.c

clean:
	@echo $(INDENT)clean plugin ap-morphology
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/ap_morphology.o
	$(Q)rm -f *.o

# $Header: /home/cvs_repos.bak/librasch/plugins/process/ap_morphology/Makefile,v 1.6 2004/07/13 14:11:08 rasch Exp $
