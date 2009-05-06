## Makefile.am for the TeX Live subdirectory utils/pdfopen/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
INCLUDES = $(X_CFLAGS)

LDADD = $(X_LIBS) -lX11 -lXmu $(X_EXTRA_LIBS)

bin_PROGRAMS = pdfopen pdfclose

pdfopen_SOURCES = pdfopen.c sendx.c

pdfclose_SOURCES = pdfclose.c sendx.c

EXTRA_DIST = xpdfopen.pdf xpdfopen.tex

