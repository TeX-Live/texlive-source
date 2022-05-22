## Makefile.am for the TeX Live subdirectory texk/makeindexk/
##
## Copyright (C) 2009-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
AM_CPPFLAGS = $(KPATHSEA_INCLUDES) -DUNIX -DKPATHSEA
AM_CFLAGS = $(WARNING_CFLAGS)

if !WIN32
dist_bin_SCRIPTS = mkindex
endif !WIN32

bin_PROGRAMS = makeindex

makeindex_SOURCES = \
	genind.c \
	genind.h \
	mkind.c \
	mkind.h \
	qsort.c \
	qsort.h \
	scanid.c \
	scanid.h \
	scanst.c \
	scanst.h \
	sortid.c

$(makeindex_OBJECTS): $(KPATHSEA_DEPEND)

LDADD = $(KPATHSEA_LIBS)	

## Rebuild libkpathsea
@KPATHSEA_RULE@

dist_man1_MANS = makeindex.1 mkindex.1

EXTRA_DIST = CONTRIB NOTES ind-src

## Tests
##
TEST_EXTENSIONS = .pl .test
TESTS = tests/nested-range-test.pl tests/makeindex.test
tests/nested-range-test.log tests/makeindex.log: makeindex$(EXEEXT)
EXTRA_DIST += $(TESTS)

## tests/nested-range-test.pl
EXTRA_DIST += tests/nested-range.tex tests/nested-range.idx \
	tests/nested-range-bb.tex tests/nested-range-bb.idx
DISTCLEANFILES = nested-range.ilg nested-range.ind
## tests/makeindex.test
EXTRA_DIST += tests/sample.idx tests/sample.ind \
	tests/range.idx tests/range2.ist tests/range2.ist tests/range3.ist \
	tests/range1.ind tests/range2.ind tests/range3.ind \
	tests/pprec0.ist tests/pprec1.ist tests/pprec2.ist tests/pprec3.ist \
	tests/pprec4.ist tests/pprec5.ist tests/pprec6.ist tests/pprec7.ist \
	tests/pprecA.idx tests/pprecA-0.ind tests/pprecA-1.ind tests/pprecA-2.ind \
	tests/pprecB.idx tests/pprecB-0.ind tests/pprecB-3.ind tests/pprecB-4.ind \
	tests/romalpA.idx tests/romalpA-5.ind tests/romalpA-6.ind \
	tests/romalpB.idx tests/romalpB-5.ind tests/romalpB-6.ind tests/romalpB-7.ind \
	tests/romalpC.idx tests/romalpC-5.ind \
	tests/romalpD.idx tests/romalpD-5.ind tests/romalpD-6.ind tests/romalpD-7.ind
DISTCLEANFILES += sample.* range*.* pprec*.* romalp*.*

