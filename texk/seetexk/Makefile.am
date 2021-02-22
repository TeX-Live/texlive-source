## Makefile.am for the TeX Live subdirectory texk/seetexk/
##
## Copyright (C) 2009-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
AM_CPPFLAGS = $(KPATHSEA_INCLUDES) -DUNIX -DKPATHSEA -DASCIIPTEX
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
	types.h \
	common.h

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
seetexk.log: dvibook$(EXEEXT) dviconcat$(EXEEXT) dviselect$(EXEEXT) dvitodvi$(EXEEXT)
EXTRA_DIST = $(TESTS)

## seetexk.test
EXTRA_DIST += \
	tests/play.dvi tests/playbook.2 tests/playconc.2 \
	tests/playdvi.2 tests/playnot.2 tests/playsel.2 \
	tests/playtate.dvi tests/playbook.3 tests/playbook.4 \
	tests/playconc.3 tests/playdvi.3 tests/playsel.3
DISTCLEANFILES = play*.2 play*.3 play*.4 play*.dvi

