LIBRASCH_HOME = ../..
include $(LIBRASCH_HOME)/Makefile.incl

CFLAGS += -fpic -I$(LIBRASCH_HOME)/include

#OBJS = eval.o event_prop.o event.o template.o load_xml.o save_xml.o mime64.o
OBJS = eval.o eval_internal.o mime64.o read_evaluation.o eval_auto.o load_eval_v0.o save_eval_v1.o load_eval_v1.o


all: ${OBJS}

eval.o: eval.c ../../include/ra.h ../../include/ra_priv.h ../../include/ra_defines.h \
	 ../../include/ra_eval.h ../../include/pl_general.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q) ${CC} ${CFLAGS} eval.c

eval_internal.o: eval_internal.c $(LIBRASCH_HOME)/include/ra_eval.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q) ${CC} ${CFLAGS} eval_internal.c

eval_auto.o: eval_auto.c $(LIBRASCH_HOME)/include/ra_eval.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q) ${CC} ${CFLAGS} eval_auto.c

load_eval_v0.o: load_eval_v0.c ../../include/ra.h ../../include/ra_priv.h ../../include/ra_defines.h \
	 ../../include/ra_eval.h ../../include/pl_general.h ../../include/eval_file_v0.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q) ${CC} ${CFLAGS} load_eval_v0.c

save_eval_v1.o: save_eval_v1.c ../../include/ra.h ../../include/ra_priv.h ../../include/ra_defines.h \
	 ../../include/ra_eval.h ../../include/pl_general.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q) ${CC} ${CFLAGS} save_eval_v1.c

load_eval_v1.o: load_eval_v1.c ../../include/ra.h ../../include/ra_priv.h ../../include/ra_defines.h \
	 ../../include/ra_eval.h ../../include/pl_general.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q) ${CC} ${CFLAGS} load_eval_v1.c

#event_prop.o: event_prop.c ../../include/ra.h ../../include/ra_priv.h ../../include/ra_defines.h \
#	 ../../include/ra_eval.h ../../include/pl_general.h
#	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
#	$(Q) ${CC} ${CFLAGS} event_prop.c
#
#event.o: event.c ../../include/ra.h ../../include/ra_priv.h ../../include/ra_defines.h \
#	 ../../include/ra_eval.h ../../include/pl_general.h
#	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
#	$(Q) ${CC} ${CFLAGS} event.c
#
#template.o: template.c ../../include/ra.h ../../include/ra_priv.h ../../include/ra_defines.h \
#	 ../../include/ra_eval.h ../../include/pl_general.h
#	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
#	$(Q) ${CC} ${CFLAGS} template.c

mime64.o: mime64.c mime64.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q) ${CC} ${CFLAGS} mime64.c

read_evaluation.o: read_evaluation.c $(LIBRASCH_HOME)/include/ra_eval.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q) ${CC} ${CFLAGS} read_evaluation.c

clean:
	$(Q)rm -f *~
	$(Q)rm -f *.o

distclean: clean

indent:
	../../scripts/Lindent *.[ch]
