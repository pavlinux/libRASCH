LIBRASCH_HOME = ..
include $(LIBRASCH_HOME)/Makefile.incl

CFLAGS += -fpic -I$(LIBRASCH_HOME)/include
LDFLAGS += -shared -Wl,-soname,librasch.so.$(MAJ_VERSION)
#LIBS = -ldl $(XML2_LIBS) -lsqlite3
LIBS = -ldl $(XML2_LIBS)

PRG = librasch.so.$(VERSION)

OBJS = ra.o handle_plugin.o pl_comm.o linked_list.o estimate_ch_infos.o value.o raw_process.o
OBJS_EVAL = ./eval/eval.o ./eval/eval_internal.o ./eval/mime64.o ./eval/read_evaluation.o \
		./eval/eval_auto.o ./eval/load_eval_v0.o ./eval/save_eval_v1.o ./eval/load_eval_v1.o
OBJS_SHARED = ./shared/pl_general.o ./shared/ra_file.o ./shared/i18n.o


all: $(OBJS) FORCE
	$(Q)cd shared && $(MAKE) $(QS) RELDIR=$(RELDIR)/shared
	$(Q)cd eval && ${MAKE} $(QS) RELDIR=$(RELDIR)/eval
	@echo $(INDENT)LD $(PRG)
	@${LD} ${DEBUG} ${LDFLAGS} $(LIBS) -o $(LIBRASCH_HOME)/${PRG} ${OBJS} $(OBJS_EVAL) $(OBJS_SHARED)

ra.o: ra.c ../include/ra.h ../include/ra_priv.h ../include/ra_defines.h ../include/handle_plugin.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q) ${CC} ${CFLAGS} ${DEBUG} -DPACKAGE=$(PACKAGE_DEFINE) -DLOCALEDIR=$(LOCALEDIR_DEFINE) ra.c

handle_plugin.o: handle_plugin_linux.c ../include/ra_defines.h ../include/ra_priv.h \
	../include/ra.h ../include/handle_plugin.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q) ${CC} ${CFLAGS} ${DEBUG} handle_plugin_linux.c -o handle_plugin.o

pl_comm.o: pl_comm.c ../include/ra_pl_comm.h  ../include/ra.h ../include/ra_priv.h \
	../include/ra_defines.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q) ${CC} ${CFLAGS} ${DEBUG} pl_comm.c -o pl_comm.o

linked_list.o: linked_list.c ../include/ra_linked_list.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q) ${CC} ${CFLAGS} ${DEBUG} linked_list.c -o linked_list.o

estimate_ch_infos.o: estimate_ch_infos.c ../include/ra_estimate_ch_infos.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q) ${CC} ${CFLAGS} ${DEBUG} estimate_ch_infos.c -o estimate_ch_infos.o

value.o: value.c ../include/ra.h ../include/ra_defines.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q) $(CC) $(CFLAGS) $(DEBUG) value.c -o value.o

raw_process.o: raw_process.c ../include/ra.h ../include/ra_defines.h ../include/ra_priv.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q) $(CC) $(CFLAGS) $(DEBUG) raw_process.c -o raw_process.o

FORCE:

clean:
	@echo clean core
	@cd shared && $(MAKE) clean
	@cd eval && $(MAKE) clean
	$(Q)rm -f *~
	$(Q)rm -f *.o

distclean: clean

indent:
	cd shared && $(MAKE) indent
	cd eval && ${MAKE} indent
	../scripts/Lindent *.[ch]
