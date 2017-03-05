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

OBJS = simple_calc_dlg_qt.o simple_calc_dlg.o options_dlg_qt.o
OBJSMOC = moc_simple_calc_dlg_qt.o moc_options_dlg_qt.o

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
	@echo $(INDENT)LD $(RELDIR)/simple_calc_dlg_qt.o
	$(Q)$(LINK) $(LFLAGS) -o simple_calc_dlg_qt_t.o $(OBJS) $(OBJSMOC)
	$(Q)cp simple_calc_dlg_qt_t.o $(LIBRASCH_HOME)/plugins/bin/simple_calc_dlg_qt.o

simple_calc_dlg.o: simple_calc_dlg.cpp simple_calc_dlg.h simple_calc_dlg_qt.h

simple_calc_dlg_qt.o: simple_calc_dlg_qt.cpp simple_calc_dlg_qt.h simple_calc_dlg_ui.h moc_simple_calc_dlg_qt.cpp options_dlg_ui.h

moc_simple_calc_dlg_qt.cpp: simple_calc_dlg_qt.h
	$(MOC) simple_calc_dlg_qt.h -o moc_simple_calc_dlg_qt.cpp

simple_calc_dlg_ui.h: simple_calc_dlg.ui
	$(UIC) -o simple_calc_dlg_ui.h simple_calc_dlg.ui

options_dlg_qt.o: options_dlg_qt.cpp options_dlg_qt.h options_dlg_ui.h moc_options_dlg_qt.cpp

options_dlg_ui.h: options_dlg.ui
	$(UIC) -o options_dlg_ui.h options_dlg.ui

moc_options_dlg_qt.cpp: options_dlg_qt.h
	$(MOC) options_dlg_qt.h -o moc_options_dlg_qt.cpp

clean:
	@echo $(INDENT)clean plugin simple-calc-dlg
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/simple_calc_dlg_qt.o
	$(Q)rm -f *.o

distclean: clean
	@echo $(INDENT)distclean plugin simple-calc-dlg
	$(Q)rm -f moc*

# $Id$
