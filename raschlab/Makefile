LIBRASCH_HOME = ..
include $(LIBRASCH_HOME)/Makefile.incl


all:
#	@cd Qt4 && $(QT_QMAKE)
	@cd Qt4 && qmake
	@cd Qt4 && $(MAKE)

install:
	@cd Qt4 && $(MAKE) install

uninstall:
	@cd Qt4 && $(MAKE) uninstall

clean:
	@cd Qt4 && $(MAKE) clean

distclean: clean
	@cd Qt4 && $(MAKE) distclean
