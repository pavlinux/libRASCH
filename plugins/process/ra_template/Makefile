LIBRASCH_HOME = ../../..
include $(LIBRASCH_HOME)/Makefile.incl

OBJS = templ.o

all: $(OBJS)
	@echo $(INDENT)LD $(RELDIR)/template.o
	$(Q)$(LD) $(LDFLAGS_PL) -o template.o ${OBJS} $(LIBRASCH_HOME)/src/shared/pl_general.o
	$(Q)cp template.o $(LIBRASCH_HOME)/plugins/bin/template.o

templ.o: template.c template.h $(LIBRASCH_HOME)/include/handle_plugin.h $(LIBRASCH_HOME)/include/ra.h $(LIBRASCH_HOME)/include/ra_defines.h \
	$(LIBRASCH_HOME)/include/ra_template.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q)$(CC) $(CFLAGS) -o templ.o template.c

clean:
	@echo $(INDENT)clean plugin template
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/template.o
	$(Q)rm -f *.o

# $Id$
