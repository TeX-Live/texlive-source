## Makefile.am for the TeX Live subdirectory texk/dvidvi/
##
## Copyright (C) 2009-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
AM_CPPFLAGS = $(KPATHSEA_INCLUDES) -DKPATHSEA -DASCIIPTEX -DUNIX
AM_CFLAGS = $(WARNING_CFLAGS)

if WIN32
LDADD = $(KPATHSEA_LIBS)
endif WIN32

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
dvidvi.log: dvidvi$(EXEXXT)
EXTRA_DIST += $(TESTS)

## dvidvi.test
EXTRA_DIST += tests/play.dvi tests/playout.2 \
	tests/playtate.dvi tests/playout.3
DISTCLEANFILES = play*.2 play*.3 play*.dvi

