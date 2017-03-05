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

OBJS = view_frame_general.o view_frame_qt.o progress_dlg_qt.o
OBJSMOC = moc_view_frame_qt.o moc_progress_dlg_qt.o

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


view_frame_general.o: view_frame_general.cpp view_frame_general.h $(LIBRASCH_HOME)/include/ra_pl_comm.h $(LIBRASCH_HOME)/include/ra_defines.h

view_frame_qt.o: view_frame_qt.cpp view_frame_qt.h view_frame_general.h view_frame_general.o

progress_dlg_qt.o: progress_dlg_qt.cpp progress_dlg_qt.h progress_dlg_qt_ui.h

progress_dlg_qt_ui.h: progress_dlg_qt.ui 
	@echo $(INDENT) $($(QUIET)$(CMD_UIC))
	$(UIC) progress_dlg_qt.ui -o progress_dlg_qt_ui.h

moc_view_frame_qt.cpp: view_frame_qt.h
	@echo $(INDENT) $($(QUIET)$(CMD_MOC))
	$(MOC) view_frame_qt.h -o moc_view_frame_qt.cpp

moc_progress_dlg_qt.cpp: progress_dlg_qt.h progress_dlg_qt_ui.h
	@echo $(INDENT) $($(QUIET)$(CMD_MOC))
	$(MOC) progress_dlg_qt.h -o moc_progress_dlg_qt.cpp

moc_progress_dlg_qt.o: moc_progress_dlg_qt.cpp

clean:
	@echo $(INDENT)clean shared source for view plugins
	$(Q)rm -f *~
	$(Q)rm -f *.o

distclean: clean
	@echo $(INDENT)distclean shared source for view plugins
	$(Q)rm -f moc*
	$(Q)rm -f *_ui.h
	$(Q)rm -f *_ui.cpp

# $Id$
