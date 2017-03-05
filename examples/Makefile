#
# Set the following two constants
#   - SIGNAL_DIR: directory which contains libRASCH supported signal files
#   - SIGNAL_NAME: One signal file in $SIGNAL_DIR
#
SIGNAL_DIR = ./database
SIGNAL_NAME = 100s.hea

LIBRASCH_HOME = ..
include $(LIBRASCH_HOME)/Makefile.incl


C_EXAMPLES = init_lib open_meas handle_recs get_raw handle_eval get_events process show_sig_qt
PERL_EXAMPLES = init_lib.pl open_meas.pl handle_recs.pl get_raw.pl handle_eval.pl get_events.pl process.pl
PYTHON_EXAMPLES = init_lib.py open_meas.py handle_recs.py get_raw.py handle_eval.py get_events.py process.py
OCTAVE_EXAMPLES = init_lib.m open_meas.m handle_recs.m get_raw.m handle_eval.m get_events.m process.m

all: $(C_EXAMPLES) $(PERL_EXAMPLES) $(PYTHON_EXAMPLES)
	../scripts/indent_examples.sh *.c
	rm -rf output
	mkdir output
	LANG=en_EN.UTF-8 ./init_lib $(SIGNAL_DIR) > ./output/init_lib.c.out
	LANG=en_EN.UTF-8 perl ./init_lib.pl $(SIGNAL_DIR) > ./output/init_lib.pl.out
	LANG=en_EN.UTF-8 python ./init_lib.py $(SIGNAL_DIR) > ./output/init_lib.py.out
	LANG=en_EN.UTF-8 octave ./init_lib.m > ./output/init_lib.m.out

	LANG=en_EN.UTF-8 ./open_meas $(SIGNAL_DIR)/$(SIGNAL_NAME) > ./output/open_meas.c.out
	LANG=en_EN.UTF-8 perl ./open_meas.pl $(SIGNAL_DIR)/$(SIGNAL_NAME) > ./output/open_meas.pl.out
	LANG=en_EN.UTF-8 python ./open_meas.py $(SIGNAL_DIR)/$(SIGNAL_NAME) > ./output/open_meas.py.out
	LANG=en_EN.UTF-8 octave ./open_meas.m > ./output/open_meas.m.out

	LANG=en_EN.UTF-8 ./handle_recs $(SIGNAL_DIR)/$(SIGNAL_NAME) > ./output/handle_recs.c.out
	LANG=en_EN.UTF-8 perl ./handle_recs.pl $(SIGNAL_DIR)/$(SIGNAL_NAME) > ./output/handle_recs.pl.out
	LANG=en_EN.UTF-8 python ./handle_recs.py $(SIGNAL_DIR)/$(SIGNAL_NAME) > ./output/handle_recs.py.out
	LANG=en_EN.UTF-8 octave ./handle_recs.m > ./output/handle_recs.m.out

	LANG=en_EN.UTF-8 octave ./get_raw.m > ./output/get_raw.m.out

	LANG=en_EN.UTF-8 ./handle_eval $(SIGNAL_DIR)/$(SIGNAL_NAME) > ./output/handle_eval.c.out
	LANG=en_EN.UTF-8 perl ./handle_eval.pl $(SIGNAL_DIR)/$(SIGNAL_NAME) > ./output/handle_eval.pl.out
	LANG=en_EN.UTF-8 python ./handle_eval.py $(SIGNAL_DIR)/$(SIGNAL_NAME) > ./output/handle_eval.py.out
	LANG=en_EN.UTF-8 octave ./handle_eval.m > ./output/handle_eval.m.out

	LANG=en_EN.UTF-8 octave ./get_events.m > ./output/get_events.m.out

	LANG=en_EN.UTF-8 ./process $(SIGNAL_DIR)/$(SIGNAL_NAME) > ./output/process.c.out
	LANG=en_EN.UTF-8 perl ./process.pl $(SIGNAL_DIR)/$(SIGNAL_NAME) > ./output/process.pl.out
	LANG=en_EN.UTF-8 python ./process.py $(SIGNAL_DIR)/$(SIGNAL_NAME) > ./output/process.py.out
	LANG=en_EN.UTF-8 octave ./process.m > ./output/process.m.out

	../scripts/wrap_output.sh


init_lib: init_lib.c
	gcc -g `librasch-config --cflags` `librasch-config --libs` -o init_lib init_lib.c

open_meas: open_meas.c
	gcc -g `librasch-config --cflags` `librasch-config --libs` -o open_meas open_meas.c

handle_recs: handle_recs.c
	gcc -g `librasch-config --cflags` `librasch-config --libs` -o handle_recs handle_recs.c

get_raw: get_raw.c
	gcc -g `librasch-config --cflags` `librasch-config --libs` -o get_raw get_raw.c

handle_eval: handle_eval.c
	gcc -g `librasch-config --cflags` `librasch-config --libs` -o handle_eval handle_eval.c

get_events: get_events.c
	gcc -g `librasch-config --cflags` `librasch-config --libs` -o get_events get_events.c

process: process.c
	gcc -g `librasch-config --cflags` `librasch-config --libs` -o process process.c

show_sig_qt: show_sig_qt.cpp
	gcc -g `librasch-config --cflags`  $(QT_CFLAGS) $(QT_INCLUDE) \
		$(QT_LFLAGS) $(QT_LIBS) `librasch-config --libs` -o show_sig show_sig_qt.cpp

html: FORCE
	rm -rf html
	mkdir html
	perl ../scripts/make_html_examples.pl ./ init_lib
	perl ../scripts/make_html_examples.pl ./ open_meas
	perl ../scripts/make_html_examples.pl ./ handle_recs
	perl ../scripts/make_html_examples.pl ./ get_raw
	perl ../scripts/make_html_examples.pl ./ handle_eval
	perl ../scripts/make_html_examples.pl ./ get_events
	perl ../scripts/make_html_examples.pl ./ process

FORCE:

clean:
	rm -rf $(C_EXAMPLES)
