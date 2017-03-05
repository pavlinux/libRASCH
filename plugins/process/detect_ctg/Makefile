LIBRASCH_HOME = ../../..
include $(LIBRASCH_HOME)/Makefile.incl

OBJS = det.o simple.o

all: ${OBJS}
	@echo $(INDENT)LD $(RELDIR)/detect_ctg.o
	$(Q)$(LD) $(LDFLAGS_PL) -o detect_ctg.o ${OBJS} $(LIBRASCH_HOME)/src/shared/pl_general.o
	$(Q)cp detect_ctg.o $(LIBRASCH_HOME)/plugins/bin/detect_ctg.o

det.o: detect_ctg.c detect_ctg.h $(LIBRASCH_HOME)/include/handle_plugin.h simple.c simple.h \
       $(LIBRASCH_HOME)/include/ra.h $(LIBRASCH_HOME)/include/ra_priv.h $(LIBRASCH_HOME)/include/ra_detect_ctg.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q)$(CC) $(CFLAGS) -o det.o detect_ctg.c

simple.o: simple.c simple.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q)$(CC) $(CFLAGS) -o simple.o simple.c

clean:
	@echo $(INDENT)clean plugin detect_ctg
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/detect_ctg.o
	$(Q)rm -f *.o

# $Id$
