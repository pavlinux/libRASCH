LIBRASCH_HOME = ../..
include $(LIBRASCH_HOME)/Makefile.incl
LIBRASCH_CFLAGS = -I$(LIBRASCH_HOME)/include
LIBRASCH_LIBS = -L$(LIBRASCH_HOME) -lrasch


########## Makefile.in ##########
CC       = gcc
CXX      = g++
CFLAGS   = -pipe -fmessage-length=0 -fPIC -Wall -W -g $(LIBRASCH_CFLAGS) $(XML2_CFLAGS)
CXXFLAGS = -pipe -fmessage-length=0 -fPIC -Wall -W -g $(LIBRASCH_CFLAGS) $(XML2_CFLAGS) -Wno-non-virtual-dtor
INCPATH  = -I$(QTDIR_RA)/mkspecs/default -I. -I$(QTDIR_RA) -I/usr/include -I/usr/include/qt4 
LINK     = g++
LFLAGS   = -g
LIBS     = -L/usr/lib/ -L$(QTDIR_RA)/lib/ -L/usr/X11R6/lib/ $(LIBRASCH_LIBS) -l$(QT_LIB) -lXext -lX11 -lm  $(XML2_LIBS)
MOC      = $(QT_MOC)
UIC      = $(QT_UIC)

####### Output directory

OBJECTS_DIR = ./

####### Files

OBJECTS = 	main_qt.o \
		raschlab_qt.o \
		signal_ra.o \
		signal_ra_qt.o \
		settings.o \
		detect_progress.o

OBJMOC = 	moc_raschlab_qt.o \
		moc_detect_progress_dlg.o

TARGET   = ../raschlab_qt


####### Implicit rules

.SUFFIXES: .c .cpp .cc .cxx .C

.cpp.o:
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q)$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cc.o:
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q)$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cxx.o:
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q)$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.C.o:
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q)$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.c.o:
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q)$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

####### Build rules

all: $(OBJECTS) $(OBJMOC)
	$(Q)$(LINK) $(LFLAGS) $(LIBS) -o $(TARGET) $(OBJECTS) $(OBJMOC)


####### Compile

main_qt.o: main_qt.cpp raschlab_qt.h \
		signal_ra_qt.h \
		../general/settings.h \

raschlab_qt.o: raschlab_qt.cpp raschlab_qt.h \
		fileopen.xpm \
		filesave.xpm \
		detect_progress.h \
		signal_ra_qt.h \
		../general/settings.h \
		../general/defines.h \
		detect_progress.h


signal_ra.o: ../general/signal_ra.cpp ../general/signal_ra.h 
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q)$(CXX) -c $(CXXFLAGS) $(INCPATH) -o signal_ra.o ../general/signal_ra.cpp

signal_ra_qt.o: signal_ra_qt.cpp signal_ra_qt.h

settings.o: ../general/settings.cpp ../general/settings.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q)$(CXX) -c $(CXXFLAGS) $(INCPATH) -o settings.o ../general/settings.cpp

detect_progress_dlg_ui.h: detect_progress_dlg.ui 
	$(UIC) detect_progress_dlg.ui -o detect_progress_dlg_ui.h

detect_progress.o: detect_progress_dlg.cpp detect_progress.h detect_progress_dlg_ui.h

moc_raschlab_qt.o: moc_raschlab_qt.cpp raschlab_qt.h signal_ra_qt.h \
		../general/settings.h \

moc_detect_progress_dlg.o: moc_detect_progress_dlg.cpp detect_progress_dlg.h 

moc_raschlab_qt.cpp: $(MOC) raschlab_qt.h
	$(MOC) raschlab_qt.h -o moc_raschlab_qt.cpp

moc_detect_progress_dlg.cpp: $(MOC) detect_progress_dlg.h
	$(MOC) detect_progress_dlg.h -o moc_detect_progress_dlg.cpp


####### Install/Uninstall

install: 
	install -d $(RASCHLAB_PREFIX)
	install $(TARGET) $(RASCHLAB_PREFIX)

uninstall: 
	rm -rf $(RASCHLAB_PREFIX)

####### Clean
clean:
	@echo clean RASCHlab
	@rm -f *.o
	@rm -f *~
	@rm -f $(TARGET)

distclean: clean
	@echo distclean RASCHlab
	@rm -f moc_*
	@rm -f detect_progress_dlg.h detect_progress_dlg.cpp
