LIBRASCH_HOME = ..
include $(LIBRASCH_HOME)/Makefile.incl

PLOT_LIB = librasch_plot.so
PLOT_LIB_VERSION = 0.0.1
PLOT_LIB_MAJ_VERSION = 0

CC = gcc
CXX = g++
LD = gcc

CFLAGS = -Wall -DLINUX -pedantic -Wno-long-long 
CFLAGS_LIB = $(CFLAGS) -c -fpic 
CFLAGS_LIB += $(PLOT_LIB_CFLAGS)

LDFLAGS = -lm -lz 
LDFLAGS_LIB = $(LDFLAGS) --shared -Wl,-soname,$(PLOT_LIB).$(MAJ_VERSION)

ifeq ($(BE_VERBOSE),1)
  Q =
else
  Q = @
endif

ifeq ($(BUILD_RELEASE), 0)
  CFLAGS += -g
  CFLAGS_LIB += -g
  LDFLAGS += -g
  LDFLAGS_LIB += -g
endif


OBJS_LIB = ra_plot_lib.o scattergram.o
OBJS_DEVICE = text_device.o
ifeq "$(QT_AVAIL)" "yes"
OBJS_DEVICE += qt_device.o
endif
ifeq "$(GD_AVAIL)" "yes"
OBJS_DEVICE += gd_device.o
LDFLAGS_LIB += $(GD_LDFLAGS)
LDFLAGS_LIB += $(GD_LIBS) -lgd
endif

#OBJS_PROGS = test_lib


#all: lib progs
all: lib


INDENT = '  '


lib: $(OBJS_LIB) $(OBJS_DEVICE)
	@echo $(INDENT)LD $(PLOT_LIB)
	$(Q) rm -f librasch_plot.so*
	$(Q) $(LD) $(LDFLAGS_LIB) -o $(PLOT_LIB).$(PLOT_LIB_VERSION) $(OBJS_LIB) $(OBJS_DEVICE)
	$(Q) ln -s $(PLOT_LIB).$(PLOT_LIB_VERSION) $(PLOT_LIB)
	$(Q) ln -s $(PLOT_LIB).$(PLOT_LIB_VERSION) $(PLOT_LIB).$(PLOT_LIB_MAJ_VERSION)

ra_plot_lib.o: ra_plot_lib.c ra_plot_lib.h ra_plot_defines.h
	@echo $(INDENT)build core
	$(Q) $(CC) $(CFLAGS_LIB) -o ra_plot_lib.o ra_plot_lib.c

scattergram.o: scattergram.c scattergram.h ra_plot_lib.h ra_plot_defines.h
	@echo $(INDENT)build scattergram
	$(Q) $(CC) $(CFLAGS_LIB) -o scattergram.o scattergram.c

text_device.o: text_device.c text_device.h ra_plot_defines.h
	@echo $(INDENT)build text-device
	$(Q) $(CC) $(CFLAGS_LIB) -o text_device.o text_device.c

ifeq "$(QT_AVAIL)" "yes"
qt_device.o: qt_device.cpp qt_device.h ra_plot_defines.h
	@echo $(INDENT)build Qt-device
	$(Q) $(CXX) $(CFLAGS_LIB) $(QT_CLAGS) $(QT_INCLUDE) -o qt_device.o qt_device.cpp
endif

ifeq "$(GD_AVAIL)" "yes"
gd_device.o: gd_device.c gd_device.h ra_plot_defines.h
	@echo $(INDENT)build libgd-device
	$(Q) $(CC) $(CFLAGS_LIB) $(GD_CFLAGS) -o gd_device.o gd_device.c
endif

# progs: $(OBJS_PROGS)

# test_lib: test_lib.cpp ra_plot_lib.h
# 	$(Q) $(CXX) -g $(CFLAGS) $(QT_CLAGS) $(QT_INCLUDE) $(QT_LIBS) $(LDFLAGS) -L. `librasch-config --cflags` `librasch-config --libs` -L. -lrasch_plot -lraschpp -o test_lib test_lib.cpp

install:
	install librasch_plot.so.$(PLOT_LIB_VERSION) $(PREFIX)/lib
	cd $(PREFIX)/lib && ln -sf librasch_plot.so.$(PLOT_LIB_VERSION) librasch_plot.so.$(PLOT_LIB_MAJ_VERSION)
	cd $(PREFIX)/lib && ln -sf librasch_plot.so.$(PLOT_LIB_VERSION) librasch_plot.so
	install --mode=644 ra_plot_lib.h $(PREFIX)/include/librasch
	install --mode=644 ra_plot_defines.h $(PREFIX)/include/librasch

clean:
	@echo $(INDENT)clean plot-lib
	$(Q) rm -f *~
	$(Q) rm -f *.o
	$(Q) rm -f librasch_plot.so*
	$(Q) rm -f test_lib

distclean: clean

# $Id: $
