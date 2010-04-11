## Makefile.am for the TeX Live subdirectory texk/mendexk/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

INCLUDES = $(KPATHSEA_INCLUDES) $(PTEXENC_INCLUDES)
AM_CPPFLAGS = -DKPATHSEA -DKPATHSEA3
AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = mendex

mendex_SOURCES = \
	convert.c \
	exkana.h \
	exvar.h \
	fread.c \
	fwrite.c \
	kana.h \
	kp.c \
	kp.h \
	ktable.h \
	main.c \
	mendex.h \
	pageread.c \
	sort.c \
	styfile.c \
	var.h


$(mendex_OBJECTS): $(KPATHSEA_DEPEND) $(PTEXENC_DEPEND)

LDADD = $(PTEXENC_LIBS) $(KPATHSEA_LIBS)	

## Rebuild libkpathsea & libptexenc
@PTEXENC_RULE@

dist_man1_MANS = mendex.1

EXTRA_DIST = COPYRIGHT COPYRIGHT.jis ChangeLog README

