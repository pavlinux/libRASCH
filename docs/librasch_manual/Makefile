include ../../Makefile.incl

INST_DIR = $(DOC_PREFIX)/librasch/librasch_manual

all:
	cd src && $(MAKE)

install:
	install -d $(INST_DIR) $(INST_DIR)/pics $(INST_DIR)/stylesheet-images $(DOC_PREFIX)/librasch/images/callouts
	install --mode=644 librasch_manual/*.html $(INST_DIR)
	install --mode=644 librasch_manual/*.css $(INST_DIR)
	install --mode=644 librasch_manual/*.manifest $(INST_DIR)
	install --mode=644 librasch_manual/pics/*.png $(INST_DIR)/pics
	install --mode=644 librasch_manual/stylesheet-images/*.gif $(INST_DIR)/stylesheet-images
	install --mode=644 images/callouts/*.gif $(DOC_PREFIX)/librasch/images/callouts
	install --mode=644 librasch_manual.pdf $(DOC_PREFIX)/librasch

clean:
	cd src && $(MAKE) clean

distclean: clean

