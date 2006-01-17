default: pdf ps dvi clean

TETEXDOC.pdf: TETEXDOC.tex
	pdflatex TETEXDOC
	bibtex TETEXDOC
	pdflatex TETEXDOC
	pdflatex TETEXDOC
	thumbpdf TETEXDOC
	pdflatex TETEXDOC

TETEXDOC.dvi: TETEXDOC.tex
	latex TETEXDOC
	bibtex TETEXDOC
	latex TETEXDOC
	latex TETEXDOC

TETEXDOC.ps: TETEXDOC.dvi
	dvips TETEXDOC -o

clean:
	rm -f TETEXDOC.aux TETEXDOC.log TETEXDOC.lot thumb???.png TETEXDOC.out TETEXDOC.tex~ TETEXDOC.toc thumbdta.tex thumbpdf.log thumbpdf.pdf TETEXDOC.blg TETEXDOC.bbl TETEXDOC.tpt auto

veryclean: clean
	rm -f TETEXDOC.pdf TETEXDOC.dvi TETEXDOC.ps

pdf: TETEXDOC.pdf
dvi: TETEXDOC.dvi
ps: TETEXDOC.ps
