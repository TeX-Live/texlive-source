## Makefile.am for the TeX Live subdirectory texk/afm2pl/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

INCLUDES = $(KPATHSEA_INCLUDES)
AM_CPPFLAGS = -DKPATHSEA
AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = afm2pl

afm2pl_SOURCES = afm2pl.c

afm2pl_DEPENDENCIES = $(KPATHSEA_DEPEND)

LDADD = $(KPATHSEA_LIBS)

## Rebuild libkpathsea
@KPATHSEA_RULE@

dist_man1_MANS = afm2pl.1

TESTS = afm2pl.test

EXTRA_DIST = afm2pl.test logo10.afm xlogo10.map xlogo10.pl

CLEANFILES = logo10.map logo10.pl

## Not used
##
EXTRA_DIST += \
	afm2pl.xml README.DOC

