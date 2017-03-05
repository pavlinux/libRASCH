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

OBJS = ra_plot.o ra_plot_qt.o ra_plot_general.o pair_opt_dlg_qt.o sel_data_dlg_qt.o
OBJSMOC = moc_ra_plot_qt.o moc_pair_opt_dlg_qt.o moc_sel_data_dlg_qt.o

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
	@echo $(INDENT)LD $(RELDIR)/ra_plot_qt.o
	$(Q)$(LINK) $(LFLAGS) -o ra_plot_qt_t.o $(OBJS) $(OBJSMOC)
	$(Q)cp ra_plot_qt_t.o $(LIBRASCH_HOME)/plugins/bin/ra_plot_qt.o

ra_plot.o: ra_plot.cpp ra_plot.h ra_plot_general.h sel_data_dlg_ui.h $(LIBRASCH_HOME)/include/ra.h $(LIBRASCH_HOME)/include/ra_defines.h

ra_plot_general.o: ra_plot_general.cpp ra_plot_general.h $(LIBRASCH_HOME)/include/ra_plot.h

ra_plot_qt.o: ra_plot_qt.cpp ra_plot_qt.h ra_plot_general.h $(LIBRASCH_HOME)/include/ra_plot.h \
	pair_opt_dlg_qt.h pair_opt_dlg_ui.h sel_data_dlg_qt.h sel_data_dlg_ui.h

pair_opt_dlg_qt.o: pair_opt_dlg_qt.cpp pair_opt_dlg_qt.h pair_opt_dlg_ui.h $(LIBRASCH_HOME)/include/ra_plot.h

sel_data_dlg_qt.o: sel_data_dlg_qt.cpp sel_data_dlg_qt.h sel_data_dlg_ui.h $(LIBRASCH_HOME)/include/ra_plot.h


moc_pair_opt_dlg_qt.cpp: pair_opt_dlg_qt.h
	@echo $(INDENT) $($(QUIET)$(CMD_MOC))
	$(MOC) pair_opt_dlg_qt.h -o moc_pair_opt_dlg_qt.cpp

moc_sel_data_dlg_qt.cpp: sel_data_dlg_qt.h
	@echo $(INDENT) $($(QUIET)$(CMD_MOC))
	$(MOC) sel_data_dlg_qt.h -o moc_sel_data_dlg_qt.cpp

moc_ra_plot_qt.cpp: ra_plot_qt.h
	@echo $(INDENT) $($(QUIET)$(CMD_MOC))
	$(MOC) ra_plot_qt.h -o moc_ra_plot_qt.cpp

pair_opt_dlg_ui.h: pair_opt_dlg.ui
	@echo $(INDENT) $($(QUIET)$(CMD_UIC))
	$(UIC) -o pair_opt_dlg_ui.h pair_opt_dlg.ui

sel_data_dlg_ui.h: sel_data_dlg.ui
	@echo $(INDENT) $($(QUIET)$(CMD_UIC))
	$(UIC) -o sel_data_dlg_ui.h sel_data_dlg.ui

clean:
	@echo $(INDENT)clean plugin plot-view
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/ra_plot_qt.o
	$(Q)rm -f *.o

distclean: clean
	@echo $(INDENT)distclean plugin plot-view
	$(Q)rm -f moc*
	$(Q)rm -f *_ui.h
	$(Q)rm -f *_ui.cpp

# $Id$
