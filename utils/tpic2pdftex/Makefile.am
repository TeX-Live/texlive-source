## Makefile.am for the TeX Live subdirectory utils/tpic2pdftex/
##
## Copyright (C) 2009-2014 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
if !WIN32
bin_SCRIPTS = tpic2pdftex
man1_MANS = tpic2pdftex.1
endif !WIN32
EXTRA_DIST = tpic2pdftex tpic2pdftex.1

pic_files = beamerexample.pic example.pic
tpic_files = $(pic_files:.pic=.tpic)
tex_files = $(pic_files:.pic=.tex)
pdf_files = $(pic_files:.pic=.pdf)

doc: $(pdf_files)

example.pdf: example.tex
	pdftex $<

beamerexample.pdf: beamerexample.tex
	pdflatex $<

%.tex: %.tpic
	$(srcdir)/tpic2pdftex $< > $@

$(tpic_files): Makefile

%.tpic: %.pic
	pic -z -t $< > $@

dist_check_SCRIPTS = tpic2pdftex.test

if !WIN32
TESTS = tpic2pdftex.test
endif !WIN32

## Expected test output
##
exp_files = $(pic_files:.pic=.exp)

CLEANFILES = $(pic_files:.pic=.log) *.aux *.nav *.out *.snm *.toc

DISTCLEANFILES = $(pdf_files) $(tex_files) $(tpic_files)

EXTRA_DIST += $(exp_files) $(pic_files)

