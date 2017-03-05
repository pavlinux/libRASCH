include ../Makefile.incl

TRANSLATIONS = de.mo es.mo


all: $(TRANSLATIONS)

%.mo : %.po
	msgfmt -o $@ $<

install:
	install -d $(LOCALEDIR)/de/LC_MESSAGES
	install de.mo $(LOCALEDIR)/de/LC_MESSAGES/$(PACKAGE).mo
	install -d $(LOCALEDIR)/es/LC_MESSAGES
	install es.mo $(LOCALEDIR)/es/LC_MESSAGES/$(PACKAGE).mo

uninstall:
	rm -f $(LOCALEDIR)/es/LC_MESSAGES/$(PACKAGE).mo

# $Header: /home/cvs_repos.bak/librasch/po/Makefile,v 1.3 2004/07/13 14:11:09 rasch Exp $
