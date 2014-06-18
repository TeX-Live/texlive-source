## Makefile.am for the TeX Live subdirectory texk/dvipos/
##
## Copyright (C) 2009-2014 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
AM_CPPFLAGS = $(KPATHSEA_INCLUDES) -DKPATHSEA
AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = dvipos

dvipos_SOURCES = \
	dvicore.c \
	dvicore.h \
	dvipos.c \
	tfm.c \
	tfm.h \
	utils.c \
	utils.h

$(dvipos_OBJECTS): $(KPATHSEA_DEPEND)

LDADD = $(KPATHSEA_LIBS)

## Rebuild libkpathsea
@KPATHSEA_RULE@

dist_man1_MANS = dvipos.1

## Not used
##
EXTRA_DIST = miktex.mak

