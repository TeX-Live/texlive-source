## Makefile.am for the TeX Live subdirectory texk/seetexk/
##
## Copyright (C) 2009-2014 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
AM_CPPFLAGS = $(KPATHSEA_INCLUDES) -DUNIX -DKPATHSEA
AM_CFLAGS = $(WARNING_CFLAGS)

noinst_LIBRARIES = libtex.a

libtex_a_SOURCES = \
	dviclass.c \
	dviclass.h \
	error.c \
	error.h \
	fio.c \
	fio.h \
	font.h \
	gripes.h \
	gripes0.c \
	gripes1.c \
	search.c \
	search.h \
	seek.h \
	seek.c \
	tempfile.c \
	tempfile.h \
	types.h

$(libtex_a_OBJECTS): $(KPATHSEA_DEPEND)

bin_PROGRAMS = dvibook dviconcat dviselect dvitodvi

$(dvibook_OBJECTS) $(dviconcat_OBJECTS) $(dviselect_OBJECTS) $(dvitodvi_OBJECTS): libtex.a

LDADD = libtex.a $(KPATHSEA_LIBS)

noinst_HEADERS = \
	dvicodes.h

dist_man1_MANS = dvibook.man dviconcat.man dviselect.man dvitodvi.man

## Rebuild libkpathsea
@KPATHSEA_RULE@

## Tests
##
TESTS = seetexk.test
EXTRA_DIST = $(TESTS)

## seetexk.test
EXTRA_DIST += \
	tests/play.dvi tests/playbook.2 tests/playconc.2 \
	tests/playdvi.2 tests/playnot.2 tests/playsel.2
DISTCLEANFILES = play*.2 play*.dvi

