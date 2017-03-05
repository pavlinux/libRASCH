include ../../Makefile.incl

INST_DIR = $(DOC_PREFIX)/librasch/raschlab_tutorial

all:
	cd src && $(MAKE)

install:
	install -d $(INST_DIR) $(INST_DIR)/pics $(INST_DIR)/stylesheet-images
	install --mode=644 raschlab_tutorial/*.html $(INST_DIR)
	install --mode=644 raschlab_tutorial/*.css $(INST_DIR)
	install --mode=644 raschlab_tutorial/*.manifest $(INST_DIR)
	install --mode=644 raschlab_tutorial/pics/*.png $(INST_DIR)/pics
	install --mode=644 raschlab_tutorial/stylesheet-images/*.gif $(INST_DIR)/stylesheet-images
	install --mode=644 raschlab_tutorial.pdf $(DOC_PREFIX)/librasch

clean:
	cd src && $(MAKE) clean

distclean: clean

