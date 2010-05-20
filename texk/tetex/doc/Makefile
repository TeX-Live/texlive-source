# Originally written by Thomas Esser.
# Public domain.
# 
default: pdf clean

TETEXDOC.pdf: TETEXDOC.tex
	pdflatex TETEXDOC
	bibtex TETEXDOC
	pdflatex TETEXDOC
	pdflatex TETEXDOC
	thumbpdf TETEXDOC
	pdflatex TETEXDOC

clean:
	rm -f TETEXDOC.aux TETEXDOC.log TETEXDOC.lot thumb???.png TETEXDOC.out TETEXDOC.tex~ TETEXDOC.toc thumbdta.tex thumbpdf.log thumbpdf.pdf TETEXDOC.blg TETEXDOC.bbl TETEXDOC.tpt auto

veryclean: clean
	rm -f TETEXDOC.pdf

pdf: TETEXDOC.pdf
