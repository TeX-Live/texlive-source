## Makefile.am for the TeX Live subdirectory texk/bibtex8/
##
## Copyright (C) 2009-2012 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

AM_CPPFLAGS = $(KPATHSEA_INCLUDES) -DUNIX -DKPATHSEA -DSUPPORT_8BIT
AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = bibtex8

bibtex8_SOURCES = \
	bibtex-1.c \
	bibtex-2.c \
	bibtex-3.c \
	bibtex-4.c \
	bibtex.c \
	bibtex.h \
	datatype.h \
	gblprocs.h \
	gblvars.h \
	utils.c \
	utils.h \
	version.h \
	sysdep.h

$(bibtex8_OBJECTS): $(KPATHSEA_DEPEND)

LDADD = $(KPATHSEA_LIBS)	

## Rebuild libkpathsea
@KPATHSEA_RULE@

csfdir = $(datarootdir)/texmf-dist/bibtex/csf/base

dist_csf_DATA = \
	csf/88591lat.csf \
	csf/88591sca.csf \
	csf/ascii.csf \
	csf/cp437lat.csf \
	csf/cp850lat.csf \
	csf/cp850sca.csf \
	csf/cp866rus.csf \
	csf/csfile.txt

btdocdir = $(datarootdir)/texmf/doc/bibtex8

dist_btdoc_DATA = \
	00readme.txt \
	HISTORY \
	csfile.txt \
	file_id.diz

## Tests.
##
TESTS = tests/bibtex.test tests/sort.test tests/bibtex-mem.test
EXTRA_DIST = $(TESTS)
DISTCLEANFILES =
## tests/bibtex.test
DISTCLEANFILES += tests/xexampl.aux tests/xexampl.bbl tests/xexampl.blg
## tests/sort.test
EXTRA_DIST += tests/sort.aux \
	tests/sort1.bbl tests/sort2.bbl tests/sort3.bbl \
	tests/sort1.csf tests/sort2.csf tests/sort3.csf \
	tests/testdata.bib tests/teststyle.bst
DISTCLEANFILES += tests/xsort.aux tests/xsort.bbl tests/xsort.blg
## tests/bibtex-mem.test
DISTCLEANFILES += tests/memtest.bib tests/memtest?.*

## Not used
##
EXTRA_DIST += \
	bt371csf.zip \
	dos-dj.mak \
	dos-emx.mak \
	os2.mak \
	unix.mak \
	csf/00readme.txt \
	csf/COPYING \
	csf/HISTORY \
	csf/file_id.diz

