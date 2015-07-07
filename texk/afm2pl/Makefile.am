## Makefile.am for the TeX Live subdirectory texk/afm2pl/
##
## Copyright (C) 2009-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
AM_CPPFLAGS = $(KPATHSEA_INCLUDES)
AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = afm2pl

afm2pl_SOURCES = afm2pl.c

$(afm2pl_OBJECTS): config.force

config.force: $(KPATHSEA_DEPEND)
	echo timestamp >config.force
	$(SHELL) ./config.status --recheck

DISTCLEANFILES = config.force

LDADD = $(KPATHSEA_LIBS)

## Rebuild libkpathsea
@KPATHSEA_RULE@

dist_man1_MANS = afm2pl.1

## Not used
##
EXTRA_DIST = \
	afm2pl.xml README.DOCS

## Tests
##
TESTS = afm2pl.test
afm2pl.log: afm2pl$(EXEEXT)
EXTRA_DIST += $(TESTS)

## afm2pl.test
EXTRA_DIST += tests/logo10.afm tests/logo10.map tests/logo10.pl
DISTCLEANFILES += logo10.*

