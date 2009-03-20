##
ACLOCAL_AMFLAGS = -I ../../m4

INCLUDES = $(KPATHSEA_INCLUDES)
AM_CPPFLAGS = -DKPATHSEA

bin_PROGRAMS = afm2pl

afm2pl_SOURCES = afm2pl.c

afm2pl_DEPENDENCIES = $(KPATHSEA_DEPEND)

LDADD = $(KPATHSEA_LIBS)	

## Rebuild libkpathsea
@KPATHSEA_RULE@

dist_man1_MANS = afm2pl.1

## Not (yet) used
##
EXTRA_DIST = \
	afm2pl.html \
	afm2pl.txt \
	afm2pl.xml \
	c-auto.in

## Eventually delete these files
##

EXTRA_DIST += Makefile.in.orig configure.in.orig

