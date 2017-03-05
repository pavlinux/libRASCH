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

OBJS = eval_dlg.o eval_dlg_qt.o evprop_dlg_qt.o
OBJSMOC = moc_eval_dlg_qt.o moc_evprop_dlg_qt.o

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
	@echo $(INDENT)LD $(RELDIR)/eval_dlg_qt.o
	$(Q)$(LINK) $(LFLAGS) -o eval_dlg_qt_t.o $(OBJS) $(OBJSMOC)
	$(Q)cp eval_dlg_qt_t.o $(LIBRASCH_HOME)/plugins/bin/eval_dlg_qt.o

eval_dlg.o: eval_dlg.cpp eval_dlg.h eval_dlg_qt.h evprop_dlg_qt.h \
		$(LIBRASCH_HOME)/include/ra.h $(LIBRASCH_HOME)/include/ra_priv.h \
		$(LIBRASCH_HOME)/include/ra_defines.h $(LIBRASCH_HOME)/include/pl_general.h

eval_dlg_ui.o: eval_dlg_ui.cpp eval_dlg_ui.h

eval_dlg_qt.o: eval_dlg_qt.cpp eval_dlg_qt.h eval_dlg_ui.h \
		$(LIBRASCH_HOME)/include/ra.h $(LIBRASCH_HOME)/include/ra_priv.h \
		$(LIBRASCH_HOME)/include/ra_defines.h $(LIBRASCH_HOME)/include/pl_general.h

eval_dlg_qt.h: eval_dlg_ui.h

moc_eval_dlg_qt.cpp: eval_dlg_qt.h
	@echo $(INDENT) $($(QUIET)$(CMD_MOC))
	$(MOC) eval_dlg_qt.h -o moc_eval_dlg_qt.cpp

eval_dlg_ui.h: eval_dlg.ui
	@echo $(INDENT) $($(QUIET)$(CMD_UIC))
	$(UIC) -o eval_dlg_ui.h eval_dlg.ui

evprop_dlg_ui.o: evprop_dlg_ui.cpp evprop_dlg_ui.h

evprop_dlg_qt.o: evprop_dlg_qt.cpp evprop_dlg_qt.h evprop_dlg_ui.h \
		$(LIBRASCH_HOME)/include/ra.h $(LIBRASCH_HOME)/include/ra_priv.h \
		$(LIBRASCH_HOME)/include/ra_defines.h $(LIBRASCH_HOME)/include/pl_general.h

evprop_dlg_qt.h: evprop_dlg_ui.h

moc_evprop_dlg_qt.cpp: evprop_dlg_qt.h
	@echo $(INDENT) $($(QUIET)$(CMD_MOC))
	$(MOC) evprop_dlg_qt.h -o moc_evprop_dlg_qt.cpp

evprop_dlg_ui.h: evprop_dlg.ui
	@echo $(INDENT) $($(QUIET)$(CMD_UIC))
	$(UIC) -o evprop_dlg_ui.h evprop_dlg.ui

clean:
	@echo $(INDENT)clean plugin eval-dlg
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/eval_dlg_qt.o
	$(Q)rm -f *.o

distclean: clean
	@echo $(INDENT)distclean plugin eval-dlg
	$(Q)rm -f moc*

# $Id$
