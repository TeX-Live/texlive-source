## Makefile.am for the TeX Live subdirectory utils/xpdfopen/
##
## Copyright (C) 2009, 2010 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

AM_CFLAGS = $(WARNING_CFLAGS)
INCLUDES = $(X_CFLAGS)

LDADD = $(X_LIBS) -lX11 -lXmu $(X_EXTRA_LIBS)

bin_PROGRAMS = pdfopen pdfclose

pdfopen_SOURCES = pdfopen.c sendx.c utils.c sendx.h xpdfopen.h utils.h

pdfclose_SOURCES = pdfclose.c sendx.c utils.c sendx.h xpdfopen.h utils.h

EXTRA_DIST = xpdfopen.pdf xpdfopen.tex

EXTRA_DIST += Makefile.am.orig configure.ac.orig