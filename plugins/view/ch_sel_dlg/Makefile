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

OBJS = ch_sel_dlg_qt.o ch_sel_dlg.o
OBJSMOC = moc_ch_sel_dlg_qt.o

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
	@echo $(INDENT)LD $(RELDIR)/ch_sel_dlg_qt.o
	$(Q)$(LINK) $(LFLAGS) -o ch_sel_dlg_qt_t.o $(OBJS) $(OBJSMOC)
	$(Q)cp ch_sel_dlg_qt_t.o $(LIBRASCH_HOME)/plugins/bin/ch_sel_dlg_qt.o

ch_sel_dlg.o: ch_sel_dlg.cpp ch_sel_dlg.h ch_sel_dlg_qt.h

ch_sel_dlg_qt.o: ch_sel_dlg_qt.cpp ch_sel_dlg_qt.h ch_sel_dlg_ui.h

moc_ch_sel_dlg_qt.cpp: ch_sel_dlg_qt.h
	@echo $(INDENT) $($(QUIET)$(CMD_MOC))
	$(MOC) ch_sel_dlg_qt.h -o moc_ch_sel_dlg_qt.cpp

ch_sel_dlg_ui.h: ch_sel_dlg.ui
	@echo $(INDENT) $($(QUIET)$(CMD_UIC))
	$(UIC) -o ch_sel_dlg_ui.h ch_sel_dlg.ui

clean:
	@echo $(INDENT)clean plugin ch-sel-dlg
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/ch_sel_dlg_qt.o
	$(Q)rm -f *.o

distclean: clean
	@echo $(INDENT)dist-clean plugin ch-sel-dlg
	$(Q)rm -f moc*
	$(Q)rm -f *_ui.h
	$(Q)rm -f *_ui.cpp

# $Id$
