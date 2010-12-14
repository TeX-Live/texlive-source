# (Public domain.  Originally written by Karl Berry, 2010.)
# This is just about creating the derived bst files, checking for
# changes, making the distribution for CTAN, etc.  It does not actually
# build a BibTeX executable.  So we keep it in the source repository.
# We're using the TL repo (source/extra/bibtex) purely for convenience.

all: pdf bst

#############################################################################
# documentation
# 
pdflatex = pdflatex --file-line-error
bibtex = bibtex

%.pdf: %.tex %.bbl
	$(pdflatex) '\nonstopmode \input $<'

%.bbl: %.tex %.aux btxdoc.bib
	$(bibtex) $*
	$(pdflatex) '\nonstopmode \input $<'

%.aux: %.tex
	$(pdflatex) '\nonstopmode \input $<'

pdf: btxdoc.pdf btxhak.pdf

.PRECIOUS: %.bbl

#############################################################################
# 
# bst files
# 
bst: plain.bst unsrt.bst alpha.bst abbrv.bst

%.bst: btxbst.doc
# First line, substitute in bare style name.
	echo "% BibTeX standard bibliography style \``basename $@ .bst`'" >$@
#
# About this cpp invocation:
# -traditional so bst integers #1, etc., don't get preprocessed away.
# -w since we know that we have unterminated ', ec.
# -P since we don't want #line in the output.
# -DBSTNAME taken from the target.
#
# Then we grep away the comments without leading whitespace,
# and fold multiple blank lines into one.
	cpp -traditional -w -P -D`basename $@ .bst|tr a-z A-Z` btxbst.doc \
	| grep -v '^%' \
	| uniq \
	>>$@
#
# For diff purposes, expand the original (which might have tabs)
# to the result (which shouldn't have any).
	-grep '	' $@
	-expand ORIG/$@ | diff - $@

#############################################################################
check:
	-grep '	' [a-z]*

dist: all
	rm -f *.blg *.log bibtex.tar.gz
	tar --exclude=\*.aux --exclude=\*.bbl -cvzf bibtex.tar.gz README *.*

clean:
	rm -f *.blg *.log *.aux *.bbl bibtex.tar.gz
	rm -f abbrv.bst alpha.bst plain.bst unsrt.bst
