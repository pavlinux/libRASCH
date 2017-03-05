include ../Makefile.incl


all: examples manual reference raschlab_tutorial source_desc

manual: version FORCE
	cd librasch_manual && $(MAKE)

reference:
	cd librasch_reference && $(MAKE)

raschlab_tutorial: version FORCE
	cd raschlab_tutorial && $(MAKE)

examples:
	cd ../examples && $(MAKE)
	cd ../examples && $(MAKE) html
	../scripts/make_html_examples.pl ../examples init_lib
	../scripts/make_html_examples.pl ../examples open_meas
	../scripts/make_html_examples.pl ../examples handle_recs
	../scripts/make_html_examples.pl ../examples get_raw
	../scripts/make_html_examples.pl ../examples handle_eval
	../scripts/make_html_examples.pl ../examples get_events
	../scripts/make_html_examples.pl ../examples process

source_desc: FORCE
	cd source_desc && doxygen librasch.doxygen

install:
	cd librasch_manual && $(MAKE) install
	cd librasch_reference && $(MAKE) install
	cd raschlab_tutorial && $(MAKE) install

version:
	../scripts/set_version.pl ../

FORCE:

clean:
	rm -f *~
	rm -f ${REFERENCES}
	cd librasch_manual && ${MAKE} clean
	cd librasch_reference && ${MAKE} clean
	cd raschlab_tutorial && ${MAKE} clean

uninstall:
	rm -rf $(DOC_PREFIX)/librasch
	rm -f $(MAN_PATH_USE)/man3/ra_*.3

# $Header: /home/cvs_repos.bak/librasch/docs/Makefile,v 1.13 2004/05/27 10:15:28 rasch Exp $
