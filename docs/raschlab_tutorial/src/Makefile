FILES = tutorial.sgml \
	preface.sgml \
	intro.sgml \
	install.sgml \
	open_signal.sgml \
	beat_detect.sgml \
	navigation.sgml \
	edit.sgml \
	signal_comments.sgml \
	settings.sgml \
	process_plugins.sgml


#all: html html_one pdf rtf
all: html_one pdf rtf

html: ${FILES}
	db2html tutorial.sgml
	mkdir ./tutorial/pics
	cp ./pics/*.png ./tutorial/pics
	rm -rf ../raschlab_tutorial
	chmod 755 tutorial
	mv ./tutorial ../raschlab_tutorial

html_one: ${FILES}
	db2html --nochunks tutorial.sgml
	cp tutorial/tutorial.html ../raschlab_tutorial_one_page.html
	cp ./pics/*.png ../pics

pdf: ${FILES}
	db2pdf tutorial.sgml
	mv tutorial.pdf ../raschlab_tutorial.pdf

rtf: ${FILES}
	db2rtf tutorial.sgml
	mv tutorial.rtf ../raschlab_tutorial.rtf

clean:
	rm -f *~
	rm -f ../raschlab_tutorial_one_page.html
	rm -f ../pics/*.png
	rm -f ../raschlab_tutorial/*.html
	rm -rf tutorial.junk
	rm -rf db2html*
	rm -f tutorial.aux
	rm -f tutorial.log
	rm -f tutorial.out
	rm -f ../raschlab_tutorial.pdf
	rm -f ../raschlab_tutorial.rtf
	rm -f tutorial.tex
