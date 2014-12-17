## Makefile.am for the TeX Live subdirectory texk/ps2pk/
##
## Copyright (C) 2009-2014 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
bin_PROGRAMS = mag pfb2pfa pk2bm ps2pk
noinst_PROGRAMS = pktest

AM_CPPFLAGS = $(KPATHSEA_INCLUDES) -DUNIX -DKPATHSEA
AM_CFLAGS = $(WARNING_CFLAGS)
LDADD = $(KPATHSEA_LIBS)	

mag_SOURCES = \
	basics.c \
	basics.h \
	mag.c

pfb2pfa_SOURCES = \
	basics.c \
	basics.h \
	filenames.c \
	filenames.h \
	pfb2pfa.c

pk2bm_SOURCES = \
	basics.c \
	basics.h \
	pk2bm.c \
	pkin.c \
	pkin.h

ps2pk_SOURCES = \
	Xstuff.h \
	arith.c \
	arith.h \
	basics.c \
	basics.h \
	blues.h \
	bstring.c \
	cluts.h \
	curves.c \
	curves.h \
	digit.h \
	encoding.c \
	ffilest.h \
	filenames.c \
	filenames.h \
	font.h \
	fontfcn.c \
	fontfcn.h \
	fontfile.h \
	fontmisc.h \
	fonts.h \
	fontstruct.h \
	fontxlfd.h \
	fsmasks.h \
	hdigit.h \
	hints.c \
	hints.h \
	lines.c \
	lines.h \
	objects.c \
	objects.h \
	paths.c \
	paths.h \
	pictures.h \
	pkout.c \
	pkout.h \
	ps2pk.c \
	psearch.h \
        regions.c \
        regions.h \
	scanfont.c \
	spaces.c \
	spaces.h \
	strokes.h \
	t1funcs.c \
	t1hdigit.h \
	t1info.c \
	t1intf.h \
	t1io.c \
	t1snap.c \
	t1stdio.h \
	token.c \
	token.h \
	tokst.h \
	type1.c \
	types.h \
	util.c \
	util.h

pktest_SOURCES = \
	basics.c \
	basics.h \
	pkout.c \
	pkout.h \
	pktest.c

$(mag_OBJECTS) $(pfb2pfa_OBJECTS) $(pk2m_OBJECTS) $(ps2pk_OBJECTS) $(pktest_OBJECTS): $(KPATHSEA_DEPEND)

## Rebuild libkpathsea
@KPATHSEA_RULE@

dist_man1_MANS = mag.1 pfb2pfa.1 pk2bm.1 ps2pk.1

## Tests.
##
TESTS_ENVIRONMENT = KPSEWHICH='$(KPSEWHICH)'
TESTS = tests/lmmath.test tests/ps2pk.test
EXTRA_DIST = $(TESTS)
DISTCLEANFILES =
## tests/lmmath.test
EXTRA_DIST += tests/lmmi12.600pk
DISTCLEANFILES += lmmi12.600pk
## tests/ps2pk.test
EXTRA_DIST += tests/Symbol.afm tests/Symbol.alpha.hex tests/Symbol.alpha.pix \
	tests/Symbol.pfa tests/Symbol.pfb tests/Symbol10.300pk
DISTCLEANFILES += Symbol*

## Not used
##
EXTRA_DIST += \
	CHANGES.type1 \
	INSTALLATION \
	README.14m \
	README.type1 \
	basename.c \
	flisearch.c \
	protos.h \
	psearch.c \
	sexpr.c \
	strexpr.c \
	strexpr.h \
	t1imager.h \
	t1stub.c \
	t1test.c \
	texfiles.c \
	texfiles.h \
	trig.h \
	usenet

