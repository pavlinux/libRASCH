LIBRASCH_HOME = ../..
include $(LIBRASCH_HOME)/Makefile.incl

CFLAGS += -fpic -I$(LIBRASCH_HOME)/include
LDFLAGS += $(XML2_LIBS)

MAKEFLAGS += --no-print-directory

all: pl_general.o ra_file.o i18n.o

pl_general.o: pl_general.c ../../include/pl_general.h ../../include/ra_defines.h \
		 ../../include/ra.h ../../include/ra_event_props.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q) ${CC} ${CFLAGS} pl_general.c -o pl_general.o

ra_file.o: ra_file.c ../../include/ra_file.h ../../include/ra_defines.h ../../include/ra.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q) ${CC} ${CFLAGS} ra_file.c -o ra_file.o

i18n.o: i18n.c ../../include/ra_priv.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q) ${CC} ${CFLAGS} i18n.c -o i18n.o

clean:
	$(Q)rm -f *~
	$(Q)rm -f *.o

distclean: clean

indent:
	../../scripts/Lindent *.[ch]
