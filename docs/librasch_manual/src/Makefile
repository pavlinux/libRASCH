FILES = manual.sgml \
	preface.sgml \
	intro.sgml \
	term.sgml \
	install.sgml 


#all: html html_one pdf rtf
all: html_one pdf rtf

html: ${FILES}
	db2html manual.sgml
	mkdir ./manual/pics
	cp pics/*.png ./manual/pics
	rm -rf ../librasch_manual
	chmod 755 manual
	mv ./manual ../librasch_manual

html_one: ${FILES}
	db2html --nochunks manual.sgml
	cp manual/manual.html ../librasch_manual_one_page.html
	rm -rf ../pics
	mkdir ../pics
	cp ./pics/*.png ../pics

pdf: ${FILES}
	db2pdf manual.sgml
	mv manual.pdf ../librasch_manual.pdf

rtf: ${FILES}
	db2rtf manual.sgml
	mv manual.rtf ../librasch_manual.rtf

clean:
	rm -f *~
	rm -f ../librasch_manual_one_page.html
	rm -f ../librasch_manual/*.html
	rm -rf manual.junk
	rm -rf db2html*
	rm -f manual.aux
	rm -f manual.log
	rm -f manual.out
	rm -f ../librasch_manual.pdf
	rm -f ../librasch_manual.rtf
	rm -f manual.tex
