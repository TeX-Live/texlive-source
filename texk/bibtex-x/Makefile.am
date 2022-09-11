## $Id$
## Makefile.am for the TeX Live subdirectory texk/bibtex-x/
##
## Copyright 2017-2022 Karl Berry <tex-live@tug.org>
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
bibtexu_LDADD = $(KPATHSEA_LIBS) $(ICU_LIBS) $(PTHREAD_LIB) $(ICU_LIBS_EXTRA)

dist_man1_MANS = bibtex8.1 bibtexu.1

## Rebuild libkpathsea
@KPATHSEA_RULE@
## Rebuild libsicuxxx
@ICU_RULE@

### BibTeX8 distribution

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
	file_id.diz \
	csf/csfile.txt

### BibTeXu distribution

btudocdir = $(datarootdir)/texmf-dist/doc/bibtexu
btuexdir = $(datarootdir)/texmf-dist/doc/bibtexu/examples

dist_btudoc_DATA = \
	01bibtexu-doc/README

dist_btuex_DATA = \
	01bibtexu-doc/examples/test.bbl \
	01bibtexu-doc/examples/test.bib \
	01bibtexu-doc/examples/test.pdf \
	01bibtexu-doc/examples/test.tex

## Tests.
##
bibtex8_tests = tests/bibtex8.test tests/bibtex8u-mem.test \
                tests/bibtex8-sort.test tests/bibtex8-char.test
bibtexu_tests = tests/bibtexu.test tests/bibtexu-basic.test \
                tests/bibtexu-range.test tests/bibtexu-yannis.test \
                tests/bibtexu-sort.test tests/bibtexu-char.test

TESTS =
if BIBTEX8
TESTS += $(bibtex8_tests)
endif BIBTEX8
tests/bibtex8.log tests/bibtex8-mem.log tests/sort.log: bibtex8$(EXEEXT)
if BIBTEXU
TESTS += $(bibtexu_tests)
endif BIBTEXU
tests/bibtexu.log tests/bibtexu-basic.log tests/bibtexu-range.log \
  tests/bibtexu-yannis.log: bibtexu$(EXEEXT)

EXTRA_DIST = $(bibtex8_tests) $(bibtexu_tests)
DISTCLEANFILES =
## tests/bibtex8.test
EXTRA_DIST += tests/exampl.bbl
DISTCLEANFILES += tests/xexampl.aux tests/xexampl.bbl tests/xexampl.blg
## tests/bibtexu.test
##
## tests/bibtex8u-mem.test
DISTCLEANFILES += tests/memtest.bib tests/memtest?.* tests/memtestu?.*
##
## tests/bibtex8-sort.test
EXTRA_DIST += tests/sort.aux \
	tests/sort1.bbl tests/sort2.bbl tests/sort3.bbl \
	tests/sort1.csf tests/sort2.csf tests/sort3.csf \
	tests/testdata.bib tests/teststyle.bst \
	tests/sort1.bbl tests/sort2.bbl tests/sort3.bbl \
	tests/casea.aux tests/casea.bbl tests/testcasea.bib tests/testcase.bst
DISTCLEANFILES += tests/xsort[123].aux tests/xsort[123].bbl tests/xsort[123].blg \
	tests/xcase[123].aux tests/xcase[123].bbl tests/xcase[123].blg
##
## tests/bibtex8-char.test
EXTRA_DIST += tests/perioda.aux tests/perioda.bbl tests/testperioda.bib tests/testperiod.bst \
	tests/substra.aux tests/substra.bbl tests/testsubstra.bib tests/testsubstr.bst \
	tests/txtprfxa.aux tests/txtprfxa.bbl tests/testtxtprfx.bst \
	tests/widtha.aux tests/widtha.bbl tests/testwidtha.bib tests/testwidth.bst \
	tests/chara.aux tests/chara.bbl tests/testchara.bib tests/testchar.bst \
	tests/namea.aux tests/namea.bbl tests/ltnbib.bib tests/testname.bst
DISTCLEANFILES += tests/xsubstra.aux tests/xsubstra.bbl tests/xsubstra.blg \
	tests/xtxtprfxa.aux tests/txtprfxa.bbl tests/xtxtprfxa.blg \
	tests/xwidtha.aux tests/widtha.bbl tests/xwidtha.blg \
	tests/xchara.aux tests/xchara.bbl tests/xchara.blg \
	tests/xnamea.aux tests/xnamea.bbl tests/xnamea.blg
##
## tests/bibtexu-basic.test
EXTRA_DIST += tests/ubasic.aux tests/ubasic.bbl tests/ubasic.bib
DISTCLEANFILES += tests/xubasic.aux tests/xubasic.bbl tests/xubasic.blg
##
## tests/bibtexu-range.test
EXTRA_DIST += tests/urange.aux tests/urange.bbl tests/urange.bib
DISTCLEANFILES += tests/xurange.aux tests/xurange.bbl tests/xurange.blg
##
## tests/bibtexu-yannis.test
EXTRA_DIST += tests/yannis.aux tests/yannis.bbl tests/yannis.bib
DISTCLEANFILES += tests/xyannis.aux tests/xyannis.bbl tests/xyannis.blg
##
## tests/bibtexu-sort.test
EXTRA_DIST += tests/sortu.aux \
	tests/sortu-az.bbl tests/sortu-da.bbl tests/sortu-en.bbl \
	tests/sortu-fi.bbl tests/sortu-tr.bbl tests/testsortu.bib \
	tests/caseu.aux tests/caseu.bbl tests/testcaseu.bib
DISTCLEANFILES += tests/xsortu-*.aux tests/xsortu-*.bbl tests/xsortu-*.blg \
	tests/xcaseu.aux tests/xcaseu.bbl tests/xcaseu.blg
##
## tests/bibtexu-char.test
EXTRA_DIST += tests/periodu.aux tests/periodu.bbl tests/testperiodu.bib \
	tests/substru.aux tests/substru.bbl tests/testsubstru.bib \
	tests/txtprfxu.aux tests/txtprfxu.bbl \
	tests/widthu.aux tests/widthu.bbl tests/testwidthu.bib \
	tests/charu.aux tests/charu.bbl tests/testcharu.bib \
	tests/nameu.aux tests/nameu.bbl tests/mlgbib.bib \
	tests/iscjku.aux tests/iscjku.bbl tests/testiscjku.bib tests/testiscjk.bst
DISTCLEANFILES += tests/xperiodu.aux tests/xperiodu.bbl tests/xperiodu.blg \
	tests/xsubstru.aux tests/xsubstru.bbl tests/xsubstru.blg \
	tests/xtxtprfxu.aux tests/txtprfxu.bbl tests/xtxtprfxu.blg \
	tests/xwidthu.aux tests/widthu.bbl tests/xwidthu.blg \
	tests/xcharu.aux tests/xcharu.bbl tests/xcharu.blg \
	tests/xnameu.aux tests/xnameu.bbl tests/xnameu.blg \
	tests/xiscjku.aux tests/xiscjku.bbl tests/xiscjku.blg

## Not used
##
EXTRA_DIST += \
	dos-dj.mak \
	dos-emx.mak \
	os2.mak \
	unix.mak
