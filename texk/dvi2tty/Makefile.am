## Makefile.am for the TeX Live subdirectory texk/dvi2tty/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

INCLUDES = $(KPATHSEA_INCLUDES)
AM_CPPFLAGS = -DUNIX -DKPATHSEA
AM_CFLAGS = $(WARNING_CFLAGS)

LDADD = $(KPATHSEA_LIBS)

bin_PROGRAMS = disdvi dvi2tty

disdvi_SOURCES = \
	commands.h \
	disdvi.c

$(disdvi_OBJECTS): $(KPATHSEA_DEPEND)

dvi2tty_SOURCES = \
	commands.h \
	dvi2tty.c \
	dvi2tty.h \
	dvistuff.c
	
$(dvi2tty_OBJECTS): $(KPATHSEA_DEPEND)

dist_man1_MANS = dvi2tty.1

## Not used
##
EXTRA_DIST = \
	DVI.format \
	MANIFEST \
	README.ORG \
	dvi2tty.patch

