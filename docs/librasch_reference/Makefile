LIBRASCH_ROOT = ../..

include $(LIBRASCH_ROOT)/Makefile.incl

FUNC_FILES = $(LIBRASCH_ROOT)/src/ra.c \
	$(LIBRASCH_ROOT)/src/raw_process.c \
	$(LIBRASCH_ROOT)/src/estimate_ch_infos.c \
	$(LIBRASCH_ROOT)/src/value.c \
	$(LIBRASCH_ROOT)/src/linked_list.c \
	$(LIBRASCH_ROOT)/src/pl_comm.c \
	$(LIBRASCH_ROOT)/src/shared/i18n.c \
	$(LIBRASCH_ROOT)/src/shared/pl_general.c \
	$(LIBRASCH_ROOT)/src/shared/ra_file.c \
	$(LIBRASCH_ROOT)/src/eval/eval.c \
	$(LIBRASCH_ROOT)/src/eval/eval_auto.c \
	$(LIBRASCH_ROOT)/src/eval/eval_internal.c \
	$(LIBRASCH_ROOT)/src/eval/read_evaluation.c \
	$(LIBRASCH_ROOT)/src/eval/load_eval_v0.c \
	$(LIBRASCH_ROOT)/src/eval/load_eval_v1.c \
	$(LIBRASCH_ROOT)/src/eval/save_eval_v1.c \
	$(LIBRASCH_ROOT)/src/eval/mime64.c


HEADER_FILES = $(LIBRASCH_ROOT)/include/ra_defines.h

INST_DIR = $(DOC_PREFIX)/librasch/librasch_reference

all: reference

reference: API.sgml API.man signals.sgml plugins.sgml infos_tab.sgml
	cd src && $(MAKE)


API.sgml: ${FUNC_FILES} FORCE
	../scripts/make_API_reference.pl -docbook -API ./${FUNC_FILES} > ./API.sgml

API.man: ${FUNC_FILES} FORCE
	../scripts/make_API_reference.pl -man -API -librasch=$(LIBRASCH_ROOT) ./${FUNC_FILES} > ./API.man
	cd ./man.3 && perl ../../scripts/cut_man_pages.pl < ../API.man

structs.sgml: ${HEADER_FILES} FORCE
#	../scripts/kernel-doc -docbook ./${HEADER_FILES} > structs.sgml

signals.sgml: ./src/signals.xml ../scripts/sig2db.pl
	../scripts/sig2db.pl ./src/signals.xml signals.sgml

plugins.sgml: ../scripts/make_module_desc.pl FORCE
	../scripts/make_module_desc.pl $(LIBRASCH_ROOT)/plugins $(LIBRASCH_ROOT)/plugins_dist plugins.sgml

infos_tab.sgml: ../scripts/infos2docbook.pl $(LIBRASCH_ROOT)/interfaces/shared/infos.h FORCE
	../scripts/infos2docbook.pl $(LIBRASCH_ROOT)/interfaces/shared/infos.h infos_tab.sgml

FORCE:

install:
	install -d $(MAN_PATH_USE)/man3
	install --mode=444 man.3/*.3 $(MAN_PATH_USE)/man3
	install -d $(INST_DIR) $(INST_DIR)/stylesheet-images
	install --mode=644 librasch_reference/*.html $(INST_DIR)
	install --mode=644 librasch_reference/*.css $(INST_DIR)
	install --mode=644 librasch_reference/*.manifest $(INST_DIR)
	install --mode=644 librasch_reference/stylesheet-images/*.gif $(INST_DIR)/stylesheet-images
	install --mode=644 librasch_reference.pdf $(DOC_PREFIX)/librasch


clean:
	rm -f API.sgml
	rm -f API.man
	rm -rf man.3/*
	rm -rf structs.sgml
	rm -rf signals.sgml
	rm -rf plugins.sgml
	rm -rf infos_tab.sgml
	rm -f *~
#	cd src && $(MAKE) clean

uninstall:


# $Header: /home/cvs_repos.bak/librasch/docs/librasch_reference/Makefile,v 1.4 2004/07/27 10:38:58 rasch Exp $
