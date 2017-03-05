LIBRASCH_HOME = ../..
include $(LIBRASCH_HOME)/Makefile.incl

CC = g++
DEBUG = -g
#CFLAGS = -c -fpic -Wall -DLINUX -I$(LIBRASCH_HOME)/include $(DEBUG) $(XML2_CFLAGS) -Weffc++
CFLAGS = -c -fpic -Wall -DLINUX -I$(LIBRASCH_HOME)/include $(DEBUG) $(XML2_CFLAGS)
LD = g++
LDFLAGS = -shared -Wl,-soname,libraschpp.so.$(MAJ_VERSION)
LIBS = -ldl $(XML2_LIBS)  -L$(LIBRASCH_HOME) -lrasch
PRG = libraschpp.so.$(VERSION)

OBJS = ra_pp.o

all: $(OBJS) FORCE
	@echo $(INDENT)build C++ interface
	$(Q)rm -f libraschpp.so*
	$(Q)${LD} ${DEBUG} ${LDFLAGS} $(LIBS) -o ${PRG} ${OBJS} $(OBJS_EVAL) $(OBJS_SHARED)
	$(Q)ln -s $(PRG) libraschpp.so
	$(Q)ln -s $(PRG) libraschpp.so.$(MAJ_VERSION)
#	ar rcs ../librasch.a $(OBJS) $(OBJS_EVAL) $(OBJS_SHARED)

ra_pp.o: ra_pp.cpp ra_pp.h
#	@echo $(INDENT) $($(QUIET)$(CMD_COMPILE))
	$(Q)${CC} ${CFLAGS} ${DEBUG} ra_pp.cpp

install:
	install -d $(PREFIX)/lib $(PREFIX)/include/librasch $(PREFIX)/bin $(PLUGIN_DIR)
	install libraschpp.so.$(VERSION) $(PREFIX)/lib
	cd $(PREFIX)/lib && ln -sf libraschpp.so.$(VERSION) libraschpp.so.$(MAJ_VERSION)
	cd $(PREFIX)/lib && ln -sf libraschpp.so.$(VERSION) libraschpp.so
	install --mode=644 ra_pp.h $(PREFIX)/include/librasch

uninstall:
	rm -f $(PREFIX)/lib/libraschpp.so*
	rm -f $(PREFIX)/bin/libraschpp-config

clean:
	@echo $(INDENT)clean C++ interface
	$(Q)rm -f *~
	$(Q)rm -f *.o
	$(Q)rm -f libraschpp.so*

distclean: clean

FORCE:


# $Id$
