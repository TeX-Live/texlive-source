##
ACLOCAL_AMFLAGS = -I ../../m4

INCLUDES = $(KPATHSEA_INCLUDES)
AM_CPPFLAGS = -DUNIX -DKPATHSEA

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

makeindex_DEPENDENCIES = $(KPATHSEA_DEPEND)

LDADD = $(KPATHSEA_LIBS)	

## Rebuild libkpathsea
@KPATHSEA_RULE@

dist_man1_MANS = makeindex.1 mkindex.1

EXTRA_DIST = ACK CONTRIB FILES NOTES PORTING

## Eventually delete these files
##

EXTRA_DIST += Makefile.in.orig configure.in.orig

