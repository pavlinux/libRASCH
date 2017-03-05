LIBRASCH_HOME = ../../..
include $(LIBRASCH_HOME)/Makefile.incl

OBJS = ideeq_ascii_t.o

all: $(OBJS)
	@echo $(INDENT)LD $(RELDIR)/ideeq_ascii.o
	$(Q)$(LD) $(LDFLAGS_PL) -o ideeq_ascii.o $(LIBRASCH_HOME)/src/shared/pl_general.o $(LIBRASCH_HOME)/src/shared/ra_file.o ${OBJS}
	$(Q)cp ideeq_ascii.o $(LIBRASCH_HOME)/plugins/bin/ideeq_ascii.o

ideeq_ascii_t.o: ideeq_ascii.c ideeq_ascii.h $(LIBRASCH_HOME)/include/handle_plugin.h $(LIBRASCH_HOME)/include/ra_endian.h $(LIBRASCH_HOME)/include/ra_file.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))	
	$(Q)$(CC) $(CFLAGS) -o ideeq_ascii_t.o ideeq_ascii.c

clean:
	@echo $(INDENT)clean plugin IDEEQ-ASCII
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/ideeq_ascii.o
	$(Q)rm -f *.o

# $Id$
