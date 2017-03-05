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


LDFLAGS += -g $(XML2_LIBS)

OBJS =		ts_view.o \
		ts_view_qt.o \
		ts_view_general.o \
		ch_optdlg_qt.o \
		ts_view_optdlg_qt.o \
		annot_dlg_qt.o

OBJSMOC = 	moc_ts_view_qt.o \
		moc_ch_optdlg_qt.o \
		moc_ts_view_optdlg_qt.o \
		moc_annot_dlg_qt.o

OBJS_SHARED = 	../shared/view_frame_general.o \
		../shared/view_frame_qt.o \
		../shared/moc_view_frame_qt.o \
		../shared/progress_dlg_qt.o \
		../shared/moc_progress_dlg_qt.o


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
	@cd ../shared && $(MAKE)
	@echo $(INDENT)LD $(RELDIR)/ts_view_qt.o
	$(Q)$(LINK) $(LFLAGS) -o ts_view_qt_t.o $(OBJS) $(OBJSMOC) $(OBJS_SHARED)
	$(Q)cp ts_view_qt_t.o $(LIBRASCH_HOME)/plugins/bin/ts_view_qt.o
ifeq "$(USE_GTK)" "yes"
	$(MAKE) -f Makefile.gtk
endif


FORCE:

ts_view.o: ts_view.cpp ts_view.h $(LIBRASCH_HOME)/include/ra.h $(LIBRASCH_HOME)/include/ra_defines.h

ch_optdlg_qt.o: ch_optdlg_qt.cpp ch_optdlg_qt.h ch_optdlg_ui.h \
	 ts_view_general.h

ts_view_general.o: ts_view_general.cpp ts_view_general.h

ts_view_qt.o: ts_view_qt.cpp ts_view_qt.h ts_view_general.h \
	 ch_optdlg_qt.h ch_optdlg_ui.h ts_view_optdlg_qt.h ts_view_optdlg_ui.h annot_dlg_qt.h annot_dlg_ui.h

ts_view_optdlg_qt.o: ts_view_optdlg_qt.cpp ts_view_optdlg_qt.h \
	 ts_view_optdlg_ui.h ts_view_general.h

annot_dlg_qt.o: annot_dlg_qt.cpp annot_dlg_qt.h \
	 annot_dlg_ui.h ts_view_general.h

moc_ch_optdlg_qt.cpp: ch_optdlg_qt.h
	@echo $(INDENT) $($(QUIET)$(CMD_MOC))
	$(Q)$(MOC) ch_optdlg_qt.h -o moc_ch_optdlg_qt.cpp

moc_ts_view_optdlg_qt.cpp: ts_view_optdlg_qt.h
	@echo $(INDENT) $($(QUIET)$(CMD_MOC))
	$(Q)$(MOC) ts_view_optdlg_qt.h -o moc_ts_view_optdlg_qt.cpp

moc_annot_dlg_qt.cpp: annot_dlg_qt.h
	@echo $(INDENT) $($(QUIET)$(CMD_MOC))
	$(Q)$(MOC) annot_dlg_qt.h -o moc_annot_dlg_qt.cpp

moc_ts_view_qt.cpp: ts_view_qt.h
	@echo $(INDENT) $($(QUIET)$(CMD_MOC))
	$(Q)$(MOC) ts_view_qt.h -o moc_ts_view_qt.cpp

ch_optdlg_ui.h: ch_optdlg.ui
	@echo $(INDENT) $($(QUIET)$(CMD_UIC))
	$(Q)$(UIC) -o ch_optdlg_ui.h ch_optdlg.ui

ts_view_optdlg_ui.h: ts_view_optdlg.ui
	@echo $(INDENT) $($(QUIET)$(CMD_UIC))
	$(Q)$(UIC) -o ts_view_optdlg_ui.h ts_view_optdlg.ui

annot_dlg_ui.h: annot_dlg.ui
	@echo $(INDENT) $($(QUIET)$(CMD_UIC))
	$(Q)$(UIC) -o annot_dlg_ui.h annot_dlg.ui

clean:
	@echo $(INDENT)clean plugin ts-view
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/ts_view_qt.o
	$(Q)rm -f *.o

distclean: clean
	@echo $(INDENT)dist-clean plugin ts-view
	$(Q)rm -f moc*
	$(Q)rm -f *_ui.h
	$(Q)rm -f *_ui.cpp

# $Id$
