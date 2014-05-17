## Makefile.am for the TeX Live subdirectory utils/xpdfopen/
##
## Copyright (C) 2009-2014 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

AM_CPPFLAGS = $(X_CFLAGS)
AM_CFLAGS = $(WARNING_CFLAGS)

LDADD = $(X_LIBS) -lX11 -lXmu $(X_EXTRA_LIBS)

bin_PROGRAMS = pdfopen pdfclose

common_sources = externs.h sendx.c sendx.h utils.c utils.h xpdfopen.h

pdfopen_SOURCES = pdfopen.c $(common_sources)

pdfclose_SOURCES = pdfclose.c $(common_sources)

dist_man1_MANS = pdfopen.1

# Symlinks within $(man1dir): FILE:LINK indicates LINK.1->FILE.1
man1_links = pdfopen:pdfclose

include $(top_srcdir)/../../am/man1_links.am

install-data-hook: install-man1-links

uninstall-hook: uninstall-man1-links

EXTRA_DIST = xpdfopen.pdf xpdfopen.tex

EXTRA_DIST += Makefile.am.orig configure.ac.orig

