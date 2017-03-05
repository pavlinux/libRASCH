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

OBJS = item_qt.o sig_sel_dlg_qt.o sig_sel_dlg.o
OBJSMOC = moc_sig_sel_dlg_qt.o

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
	@echo $(INDENT)LD $(RELDIR)/sig_sel_dlg_qt.o
	$(Q)$(LINK) $(LFLAGS) -o sig_sel_dlg_qt_t.o $(OBJS) $(OBJSMOC)
	$(Q)cp sig_sel_dlg_qt_t.o $(LIBRASCH_HOME)/plugins/bin/sig_sel_dlg_qt.o

item_qt.o: item_qt.cpp item_qt.h

sig_sel_dlg.o: sig_sel_dlg.cpp sig_sel_dlg.h $(LIBRASCH_HOME)/include/ra.h $(LIBRASCH_HOME)/include/ra_defines.h \
	$(LIBRASCH_HOME)/include/ra_priv.h $(LIBRASCH_HOME)/include/pl_general.h

sig_sel_dlg_qt.o: sig_sel_dlg_qt.cpp dir_qt.h item_qt.h \
	 sig_sel_dlg_qt.h

moc_sig_sel_dlg_qt.cpp: sig_sel_dlg_qt.h
	@echo $(INDENT) $($(QUIET)$(CMD_MOC))
	$(MOC) sig_sel_dlg_qt.h -o moc_sig_sel_dlg_qt.cpp

clean:
	@echo $(INDENT)clean plugin sig-sel-dlg
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/sig_sel_dlg_qt.o
	$(Q)rm -f *.o

distclean: clean
	@echo $(INDENT)distclean plugin sig-sel-dlg
	$(Q)rm -f moc*
	$(Q)rm -f *_ui.h
	$(Q)rm -f *_ui.cpp

# $Id$
