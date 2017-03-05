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

OBJS 	= 	ev_summary_view.o \
		ev_summary_view_qt.o \
		ev_summary_view_general.o \
		ev_summary_view_scroll_area.o

OBJSMOC = 	moc_ev_summary_view_qt.o \
		moc_ev_summary_view_scroll_area.o


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



all: $(OBJS) $(OBJSMOC)
	@echo $(INDENT)LD $(RELDIR)/ev_summary_view_qt.o
	$(Q)$(LINK) $(LFLAGS) -o ev_summary_view_qt_t.o $(OBJS) $(OBJSMOC)
	$(Q)cp ev_summary_view_qt_t.o $(LIBRASCH_HOME)/plugins/bin/ev_summary_view_qt.o


ev_summary_view.o: ev_summary_view.cpp ev_summary_view.h $(LIBRASCH_HOME)/include/ra.h $(LIBRASCH_HOME)/include/ra_defines.h

ev_summary_view_general.o: ev_summary_view_general.cpp ev_summary_view_general.h

ev_summary_view_qt.o: ev_summary_view_qt.cpp ev_summary_view_qt.h ev_summary_view_general.h

moc_ev_summary_view_qt.cpp: ev_summary_view_qt.h
	@echo $(INDENT) $($(QUIET)$(CMD_MOC))
	$(MOC) ev_summary_view_qt.h -o moc_ev_summary_view_qt.cpp

moc_ev_summary_view_scroll_area.cpp: ev_summary_view_scroll_area.h
	@echo $(INDENT) $($(QUIET)$(CMD_MOC))
	$(MOC) ev_summary_view_scroll_area.h -o moc_ev_summary_view_scroll_area.cpp

clean:
	@echo $(INDENT)clean plugin ev-summary-view
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/ev_summary_view_qt.o
	$(Q)rm -f *.o

distclean: clean
	@echo $(INDENT)distclean plugin ev-summary-view
	$(Q)rm -f moc*
	$(Q)rm -f *_ui.h
	$(Q)rm -f *_ui.cpp

# $Id$
