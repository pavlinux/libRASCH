TEMPLATE = app
VERSION = 0.0.1
CONFIG = qt warn_on debug
FORMS = detect_progress_dlg.ui
SOURCES = main_qt.cpp raschlab_qt.cpp detect_progress.cpp ../general/signal_ra.cpp signal_ra_qt.cpp ../general/settings.cpp
HEADERS = raschlab_qt.h /home/rasch/src/librasch/include/ra.h /home/rasch/src/librasch/include/ra_defines.h \
	detect_progress.h ../general/signal_ra.h signal_ra_qt.h ../general/settings.h
TARGET = raschlab_qt
DESTDIR = ../
INCLUDEPATH = /home/rasch/src/librasch/include /usr/lib/qt2/include
DEPENDPATH = /home/rasch/src/librasch/include
unix:LIBS = -L/home/rasch/src/librasch -lrasch
