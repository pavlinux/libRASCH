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

OBJS = plugin_info_dlg.o plugin_info_dlg_qt.o item_qt.o
OBJSMOC = moc_plugin_info_dlg_qt.o

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
	@echo $(INDENT)LD $(RELDIR)/plugin_info_dlg_qt.o
	$(Q)$(LINK) $(LFLAGS) -o plugin_info_dlg_t.o $(OBJS) $(OBJSMOC)
	$(Q)cp plugin_info_dlg_t.o $(LIBRASCH_HOME)/plugins/bin/plugin_info_dlg_qt.o

plugin_info_dlg.o: plugin_info_dlg.cpp plugin_info_dlg.h plugin_info_dlg_ui.h \
		$(LIBRASCH_HOME)/include/ra.h $(LIBRASCH_HOME)/include/ra_priv.h \
		$(LIBRASCH_HOME)/include/ra_defines.h $(LIBRASCH_HOME)/include/pl_general.h item_qt.cpp item_qt.h

moc_plugin_info_dlg_qt.cpp: plugin_info_dlg_qt.h
	@echo $(INDENT) $($(QUIET)$(CMD_MOC))
	$(MOC) plugin_info_dlg_qt.h -o moc_plugin_info_dlg_qt.cpp

plugin_info_dlg_ui.h: plugin_info_dlg.ui
	@echo $(INDENT) $($(QUIET)$(CMD_UIC))
	$(UIC) -o plugin_info_dlg_ui.h plugin_info_dlg.ui

clean:
	@echo $(INDENT)clean plugin plugin-info-dlg
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/plugin_info_dlg.o
	$(Q)rm -f *.o

distclean: clean
	@echo $(INDENT)distclean plugin plugin-info-dlg
	$(Q)rm -f moc*
	$(Q)rm -f *_ui.h
	$(Q)rm -f *_ui.cpp

# $Id$
