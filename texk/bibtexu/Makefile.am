## Makefile.am for the TeX Live subdirectory texk/bibtexu/
##
## Copyright (C) 2009-2012 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

AM_CPPFLAGS = $(KPATHSEA_INCLUDES) $(ICU_INCLUDES) -DUNIX -DKPATHSEA -DSUPPORT_8BIT 
AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = bibtexu

# Force Automake to use CXXLD for linking
nodist_EXTRA_bibtexu_SOURCES = dummy.cxx

bibtexu_SOURCES = \
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

$(bibtexu_OBJECTS): $(KPATHSEA_DEPEND) $(ICU_DEPEND)

LDADD = $(KPATHSEA_LIBS) $(ICU_LIBS)

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

btdocdir = $(datarootdir)/texmf/doc/bibtexu

dist_btdoc_DATA = \
	00readme.txt \
	HISTORY \
	csfile.txt \
	file_id.diz

DISTCLEANFILES = CXXLD.sh

## Tests.
##
TESTS = tests/bibtex.test tests/bibtexu.test
EXTRA_DIST = $(TESTS)

## tests/bibtex.test
DISTCLEANFILES += tests/xexampl.aux tests/xexampl.bbl tests/xexampl.blg

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

