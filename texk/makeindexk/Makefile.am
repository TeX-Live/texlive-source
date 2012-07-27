## Makefile.am for the TeX Live subdirectory texk/makeindexk/
##
## Copyright (C) 2009-2012 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

INCLUDES = $(KPATHSEA_INCLUDES)
AM_CPPFLAGS = -DUNIX -DKPATHSEA
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

EXTRA_DIST = CONTRIB NOTES

## Tests
##
TESTS = tests/nested-range-test.pl tests/makeindex.test
EXTRA_DIST += $(TESTS)

## tests/nested-range-test.pl
EXTRA_DIST += tests/nested-range.tex tests/nested-range.idx \
	tests/nested-range-bb.tex tests/nested-range-bb.idx
DISTCLEANFILES = nested-range.ilg nested-range.ind
## tests/makeindex.test
EXTRA_DIST += tests/sample.idx tests/sample.ind
DISTCLEANFILES += sample.*

