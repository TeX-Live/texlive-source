## Makefile.am for the TeX Live subdirectory texk/makeindexk/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

INCLUDES = $(KPATHSEA_INCLUDES)
AM_CPPFLAGS = -DUNIX -DKPATHSEA
AM_CFLAGS = $(WARNING_CFLAGS)

dist_bin_SCRIPTS = mkindex

bin_PROGRAMS = makeindex

makeindex_SOURCES = \
	genind.c \
	genind.h \
	mkind.c \
	mkind.h \
	qsort.c \
	scanid.c \
	scanid.h \
	scanst.c \
	scanst.h \
	sortid.c

$(makeindex_OBJECTS): $(KPATHSEA_DEPEND)

LDADD = $(KPATHSEA_LIBS)	

## Rebuild libkpathsea
@KPATHSEA_RULE@

dist_man1_MANS = makeindex.1 mkindex.1

EXTRA_DIST = ACK CONTRIB FILES NOTES PORTING

