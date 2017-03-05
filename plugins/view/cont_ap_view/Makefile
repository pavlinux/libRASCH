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

OBJS 	= 	cont_ap_view.o \
		cont_ap_view_qt.o \
		cont_ap_view_general.o \
		../ts_view/ts_view_qt.o \
		../ts_view/ts_view_general.o \
		../ts_view/ch_optdlg_qt.o \
		../ts_view/ts_view_optdlg_qt.o \
		../ts_view/annot_dlg_qt.o

OBJSMOC = 	moc_cont_ap_view_qt.o \
		../ts_view/moc_ts_view_qt.o \
		../ts_view/moc_ch_optdlg_qt.o \
		../ts_view/moc_ts_view_optdlg_qt.o \
		../ts_view/moc_annot_dlg_qt.o

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
	@echo $(INDENT)LD $(RELDIR)/cont_ap_view_qt.o
	@cd ../ts_view && $(MAKE)
	$(Q)$(LINK) $(LFLAGS) -o cont_ap_view_qt_t.o $(OBJS) $(OBJSMOC) $(OBJS_SHARED)
	$(Q)cp cont_ap_view_qt_t.o $(LIBRASCH_HOME)/plugins/bin/cont_ap_view_qt.o

FORCE:

cont_ap_view.o: cont_ap_view.cpp cont_ap_view.h $(LIBRASCH_HOME)/include/ra.h $(LIBRASCH_HOME)/include/ra_defines.h

cont_ap_view_general.o: cont_ap_view_general.cpp cont_ap_view_general.h ../ts_view/ts_view_general.h

cont_ap_view_qt.o: cont_ap_view_qt.cpp cont_ap_view_qt.h cont_ap_view_general.h ../ts_view/ts_view_qt.h ../ts_view/ts_view_general.h 

moc_cont_ap_view_qt.cpp: cont_ap_view_qt.h
	@echo $(INDENT) $($(QUIET)$(CMD_MOC))
	$(MOC) cont_ap_view_qt.h -o moc_cont_ap_view_qt.cpp

clean:
	@echo $(INDENT)clean plugin cont-ap-view
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/cont_ap_view_qt.o
	$(Q)rm -f *.o

distclean: clean
	@echo $(INDENT)distclean plugin cont-ap-view
	$(Q)rm -f moc*
	$(Q)rm -f *_ui.h
	$(Q)rm -f *_ui.cpp

# $Id$
