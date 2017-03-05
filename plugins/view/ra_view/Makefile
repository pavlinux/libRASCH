LIBRASCH_HOME = ../../..
include $(LIBRASCH_HOME)/Makefile.incl

CFLAGS	+= 	$(QT_CFLAGS) -DQT
INCPATH	=	$(QT_INCLUDE)
LINK	=	$(LD_CXX)
LFLAGS	= 	$(LDFLAGS_PL) $(QT_LDFLAGS_QT)
AR	=	ar cqs
RANLIB	=	
MOC	=	$(QT_MOC)
UIC	=	$(QT_UIC)

OBJS 	= 	ra_view.o \
		ra_view_qt.o \
		ra_view_general.o \
		ra_view_splitter_qt.o

OBJSMOC = 	moc_ra_view_qt.o moc_ra_view_splitter_qt.o

OBJS_SHARED = 	../shared/view_frame_general.o \
		../shared/view_frame_qt.o \
		../shared/moc_view_frame_qt.o


####### Implicit rules

.SUFFIXES: .cpp .cxx .cc .C .c

.cpp.o:
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q)$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cxx.o:
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q)$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cc.o:
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q)$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.C.o:
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q)$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.c.o:
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q)$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<



all: FORCE $(OBJS) $(OBJSMOC)
	@echo $(INDENT)LD $(RELDIR)/ra_view_qt.o
	@cd ../shared && $(MAKE)
	$(Q)$(LINK) $(LFLAGS) -o ra_view_qt_t.o $(OBJS) $(OBJSMOC) $(OBJS_SHARED)
	$(Q)cp ra_view_qt_t.o $(LIBRASCH_HOME)/plugins/bin/ra_view_qt.o
ifeq "$(USE_GTK)" "yes"
	$(MAKE) -f Makefile.gtk
endif

FORCE:

ra_view.o: ra_view.cpp ra_view.h $(LIBRASCH_HOME)/include/ra.h $(LIBRASCH_HOME)/include/ra_defines.h

ra_view_general.o: ra_view_general.cpp ra_view_general.h

ra_view_qt.o: ra_view_qt.cpp ra_view_qt.h ra_view_general.h

ra_view_splitter_qt.o: ra_view_splitter_qt.cpp ra_view_splitter_qt.h

moc_ra_view_qt.cpp: ra_view_qt.h
	@echo $(INDENT) $($(QUIET)$(CMD_MOC))
	$(MOC) ra_view_qt.h -o moc_ra_view_qt.cpp

moc_ra_view_splitter_qt.cpp: ra_view_splitter_qt.h
	@echo $(INDENT) $($(QUIET)$(CMD_MOC))
	$(MOC) ra_view_splitter_qt.h -o moc_ra_view_splitter_qt.cpp

clean:
	@echo $(INDENT)clean plugin ra-view
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/ra_view_qt.o
	$(Q)rm -f *.o

distclean: clean
	@echo $(INDENT)distclean plugin ra-view
	$(Q)rm -f moc*
	$(Q)rm -f *_ui.h
	$(Q)rm -f *_ui.cpp

# $Header: /home/cvs_repos.bak/librasch/plugins/view/ra_view/Makefile,v 1.10 2004/10/21 17:36:17 rasch Exp $
