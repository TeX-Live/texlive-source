## Makefile.am for the TeX Live subdirectory texk/bibtex8/
##
## Copyright (C) 2009, 2010 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

INCLUDES = $(KPATHSEA_INCLUDES)
AM_CPPFLAGS = -DUNIX -DKPATHSEA -DSUPPORT_8BIT
AM_CFLAGS = $(WARNING_CFLAGS)

## FIXME: (1) this should go away once the code has been carefully
## FIXME: analyzed and uses 'unsigned char' where necessary.
## FIXME: (2) this should go to AM_CFLAGS, but AIX xlc only
## FIXME: accepts this flag when compiling, not when linking.
AM_CPPFLAGS += -funsigned-char

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

csfdir = ${prefix}/texmf-dist/bibtex/csf/base

dist_csf_DATA = \
	csf/88591lat.csf \
	csf/88591sca.csf \
	csf/ascii.csf \
	csf/cp437lat.csf \
	csf/cp850lat.csf \
	csf/cp850sca.csf \
	csf/cp866rus.csf \
	csf/csfile.txt

btdocdir = ${prefix}/texmf/doc/bibtex8

dist_btdoc_DATA = \
	00readme.txt \
	HISTORY \
	csfile.txt \
	file_id.diz

## Not used
##
EXTRA_DIST = \
	bt371csf.zip \
	dos-dj.mak \
	dos-emx.mak \
	os2.mak \
	unix.mak \
	csf/00readme.txt \
	csf/COPYING \
	csf/HISTORY \
	csf/file_id.diz

