## $Id$
## Makefile.am for the TeX Live subdirectory texk/bibtex-x/
##
## Copyright 2017-2019 Karl Berry <tex-live@tug.org>
## Copyright 2009-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
AM_CPPFLAGS = $(KPATHSEA_INCLUDES) -DUNIX -DKPATHSEA
AM_CFLAGS = $(WARNING_CFLAGS)

EXTRA_PROGRAMS = bibtex8 bibtexu

bin_PROGRAMS =
if BIBTEX8
bin_PROGRAMS += bibtex8
endif BIBTEX8
if BIBTEXU
bin_PROGRAMS += bibtexu
endif BIBTEXU

common_sources = \
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

bibtex8_SOURCES = $(common_sources)
bibtex8_CPPFLAGS = $(AM_CPPFLAGS) -DSUPPORT_8BIT
$(bibtex8_OBJECTS): $(KPATHSEA_DEPEND)
bibtex8_LDADD = $(KPATHSEA_LIBS)


bibtexu_SOURCES = $(common_sources)
# Force Automake to use CXXLD for linking
nodist_EXTRA_bibtexu_SOURCES = dummy.cxx
bibtexu_CPPFLAGS = $(AM_CPPFLAGS) $(ICU_INCLUDES) -DUTF_8
$(bibtexu_OBJECTS): $(KPATHSEA_DEPEND) $(ICU_DEPEND)
bibtexu_LDADD = $(KPATHSEA_LIBS) $(ICU_LIBS) $(ICU_LIBS_EXTRA)

dist_man1_MANS = bibtex8.1 bibtexu.1

## Rebuild libkpathsea
@KPATHSEA_RULE@
## Rebuild libsicuxxx
@ICU_RULE@

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

btdocdir = $(datarootdir)/texmf-dist/doc/bibtex8

dist_btdoc_DATA = \
	00readme.txt \
	HISTORY \
	csfile.txt \
	file_id.diz

## Tests.
##
bibtex8_tests = tests/bibtex8.test tests/bibtex8-mem.test tests/sort.test
bibtexu_tests = tests/bibtexu.test

TESTS =
if BIBTEX8
TESTS += $(bibtex8_tests)
endif BIBTEX8
tests/bibtex8.log tests/bibtex8-mem.log tests/sort.log: bibtex8$(EXEEXT)
if BIBTEXU
TESTS += $(bibtexu_tests)
endif BIBTEXU
tests/bibtexu.log: bibtexu$(EXEEXT)

EXTRA_DIST = $(bibtex8_tests) $(bibtexu_tests)
DISTCLEANFILES =
## tests/bibtex8.test
DISTCLEANFILES += tests/xexampl.aux tests/xexampl.bbl tests/xexampl.blg
## tests/bibtexu.test
## tests/sort.test
EXTRA_DIST += tests/sort.aux \
	tests/sort1.bbl tests/sort2.bbl tests/sort3.bbl \
	tests/sort1.csf tests/sort2.csf tests/sort3.csf \
	tests/testdata.bib tests/teststyle.bst
DISTCLEANFILES += tests/xsort.aux tests/xsort.bbl tests/xsort.blg
## tests/bibtex8-mem.test
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
