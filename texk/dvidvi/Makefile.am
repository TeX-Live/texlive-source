## Makefile.am for the TeX Live subdirectory texk/dvidvi/
##
## Copyright (C) 2009-2014 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
AM_CPPFLAGS = $(KPATHSEA_INCLUDES) -DKPATHSEA -DUNIX
AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = dvidvi

dvidvi_SOURCES = dvidvi.c

dist_man1_MANS = dvidvi.1

## Rebuild libkpathsea
@KPATHSEA_RULE@

## Not used
##
EXTRA_DIST = \
	a5bookle.bat \
	a5bookle.com \
	a5bookle.hlp \
	a5test.tex \
	build.com \
	doubside.bat \
	doubside.com \
	doubside.hlp \
	dvidvi.kohm-patch \
	dvisamp.tex

## Tests
##
TESTS = dvidvi.test
EXTRA_DIST += $(TESTS)

## dvidvi.test
EXTRA_DIST += tests/play.dvi tests/playout.2
DISTCLEANFILES = play*.2 play*.dvi

