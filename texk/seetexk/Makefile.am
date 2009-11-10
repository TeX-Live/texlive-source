## Makefile.am for the TeX Live subdirectory texk/seetexk/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

INCLUDES = $(KPATHSEA_INCLUDES)
AM_CPPFLAGS = -DUNIX -DKPATHSEA
AM_CFLAGS = $(WARNING_CFLAGS)

noinst_LIBRARIES = libtex.a

libtex_a_SOURCES = \
	dviclass.c \
	dviclass.h \
	error.c \
	error.h \
	fio.c \
	fio.h \
	font.h \
	gripes.h \
	gripes0.c \
	gripes1.c \
	search.c \
	search.h \
	seek.h \
	seek.c \
	tempfile.c \
	tempfile.h \
	types.h

$(libtex_a_OBJECTS): $(KPATHSEA_DEPEND)

bin_PROGRAMS = dvibook dviconcat dviselect dvitodvi

$(dvibook_OBJECTS) $(dviconcat_OBJECTS) $(dviselect_OBJECTS) $(dvitodvi_OBJECTS): libtex.a

LDADD = libtex.a $(KPATHSEA_LIBS)

noinst_HEADERS = \
	dvicodes.h

dist_man1_MANS = dvibook.man dviconcat.man dviselect.man dvitodvi.man

## Rebuild libkpathsea
@KPATHSEA_RULE@

## Not used
##
EXTRA_DIST = \
	bcopy.c \
	bzero.c \
	conv.c \
	conv.h \
	convstruct.h \
	dvi_draw.c \
	dvistate.c \
	dvistate.h \
	dvistuff.c \
	dvistuff.h \
	findpost.c \
	font.c \
	font_subr.c \
	getopt.c \
	gfclass.c \
	gfclass.h \
	gfcodes.h \
	gffont.c \
	magfactor.c \
	num.h \
	pkfont.c \
	postamble.h \
	pxlfont.c \
	rotate.c \
	rstfont.c \
	scaletfm.c \
	scanpost.c \
	sdecode.c \
	sdecode.h \
	skip.c \
	split.c \
	strsave.c \
	strtol.c \
	tfm.c \
	tfm.h \
	tfmfont.c

