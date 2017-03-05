FILES = ref_c.sgml \
	../API.sgml \
	ref_perl.sgml \
	ref_python.sgml \
	ref_mat_oct.sgml \
	infos.sgml \
	../infos_tab.sgml \
	plugins.sgml \
	../plugins.sgml


#all: html html_one pdf rtf
all: html_one pdf rtf

html: $(FILES)
	db2html reference.sgml
	rm -rf ../librasch_reference
	chmod 755 reference
	mv reference ../librasch_reference

html_one: ${FILES}
	db2html --nochunks reference.sgml
	cp reference/reference.html ../librasch_API_ref_one_page.html

pdf: ${FILES}
	db2pdf reference.sgml
	mv reference.pdf ../librasch_reference.pdf

rtf: ${FILES}
	db2rtf reference.sgml
	mv reference.rtf ../librasch_reference.rtf

clean:
	rm -f *~
	rm -rf db2html*
	rm -rf reference.junk
	rm -f reference.aux
	rm -f reference.log
	rm -f reference.out
	rm -f reference.tex
	rm -f ../librasch_reference/*.html
	rm -f ../librasch_reference_one_page.html
	rm -f ../librasch_reference.pdf
	rm -f ../librasch_reference.rtf
