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

OBJS 	= 	annot_comment_view.o \
		annot_comment_view_qt.o \
		annot_comment_view_general.o \
		annot_comment_tree_view.o \
		annot_comment_model.o

OBJSMOC = 	moc_annot_comment_view_qt.o \
		moc_annot_comment_tree_view.o \
		moc_annot_comment_model.o


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
	@echo $(INDENT)LD $(RELDIR)/annot_comment_view_qt.o
	$(Q)$(LINK) $(LFLAGS) -o annot_comment_view_qt_t.o $(OBJS) $(OBJSMOC)
	$(Q)cp annot_comment_view_qt_t.o $(LIBRASCH_HOME)/plugins/bin/annot_comment_view_qt.o


annot_comment_view.o: annot_comment_view.cpp annot_comment_view.h $(LIBRASCH_HOME)/include/ra.h $(LIBRASCH_HOME)/include/ra_defines.h

annot_comment_view_general.o: annot_comment_view_general.cpp annot_comment_view_general.h

annot_comment_view_qt.o: annot_comment_view_qt.cpp annot_comment_view_qt.h annot_comment_view_general.h \
			 annot_comment_tree_view.h annot_comment_model.h

annot_comment_tree_view.o: annot_comment_tree_view.cpp annot_comment_tree_view.h

annot_comment_model.o: annot_comment_model.cpp annot_comment_model.h

moc_annot_comment_view_qt.cpp: annot_comment_view_qt.h
	@echo $(INDENT) $($(QUIET)$(CMD_MOC))
	$(MOC) annot_comment_view_qt.h -o moc_annot_comment_view_qt.cpp

moc_annot_comment_tree_view.cpp: annot_comment_tree_view.h
	@echo $(INDENT) $($(QUIET)$(CMD_MOC))
	$(MOC) annot_comment_tree_view.h -o moc_annot_comment_tree_view.cpp

moc_annot_comment_model.cpp: annot_comment_model.h
	@echo $(INDENT) $($(QUIET)$(CMD_MOC))
	$(MOC) annot_comment_model.h -o moc_annot_comment_model.cpp


clean:
	@echo $(INDENT)clean plugin annot-comment-view
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/annot_comment_view_qt.o
	$(Q)rm -f *.o

distclean: clean
	@echo $(INDENT)distclean plugin annot-comment-view
	$(Q)rm -f moc*
	$(Q)rm -f *_ui.h
	$(Q)rm -f *_ui.cpp

# $Id$
