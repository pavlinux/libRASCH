LIBRASCH_HOME = ../../..
include $(LIBRASCH_HOME)/Makefile.incl

OBJS = det.o main_detect.o sig.o stat.o peaks.o beat_mem.o

all: $(OBJS)
	@echo $(INDENT)LD $(RELDIR)/detect_ecg.o
	$(Q)$(LD) $(LDFLAGS_PL) -o detect_ecg.o ${OBJS} $(LIBRASCH_HOME)/src/shared/pl_general.o
	$(Q)cp detect_ecg.o $(LIBRASCH_HOME)/plugins/bin/detect_ecg.o

det.o: detect_ecg.c detect_ecg.h $(LIBRASCH_HOME)/include/handle_plugin.h main_detect.c main_detect.h \
       beat_mem.c beat_mem.h $(LIBRASCH_HOME)/include/ra.h $(LIBRASCH_HOME)/include/ra_priv.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q)$(CC) $(CFLAGS) -o det.o detect_ecg.c

main_detect.o: main_detect.c main_detect.h statistics.c statistics.h beat_mem.c beat_mem.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q)$(CC) $(CFLAGS) -o main_detect.o main_detect.c

sig.o: signal_processing.c signal_processing.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q)$(CC) $(CFLAGS) -o sig.o signal_processing.c

stat.o: statistics.c statistics.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q)$(CC) $(CFLAGS) -o stat.o statistics.c

peaks.o: get_peaks.c get_peaks.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q)$(CC) $(CFLAGS) -o peaks.o get_peaks.c

beat_mem.o: beat_mem.c beat_mem.h
	@echo $(INDENT)$($(QUIET)$(CMD_COMPILE))
	$(Q)$(CC) $(CFLAGS) -o beat_mem.o beat_mem.c

clean:
	@echo $(INDENT)clean plugin detect_ecg
	$(Q)rm -f *~
	$(Q)rm -f $(LIBRASCH_HOME)/plugins/bin/detect_ecg.o
	$(Q)rm -f *.o

# $Id$
