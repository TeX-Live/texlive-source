## Makefile.am for the TeX Live subdirectory texk/ps2pkm/
##
## Copyright (C) 2009-2011 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

INCLUDES = $(KPATHSEA_INCLUDES)
AM_CPPFLAGS = -DUNIX -DKPATHSEA
AM_CFLAGS = #$(WARNING_CFLAGS)

bin_PROGRAMS = mag pfb2pfa pk2bm ps2pk

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
	t1imager.h \
	t1info.c \
	t1intf.h \
	t1io.c \
        t1snap.c \
	t1stdio.h \
	t1stub.c \
	token.c \
	token.h \
	tokst.h \
	trig.h \
	type1.c \
	types.h \
	util.c \
	util.h

$(mag_OBJECTS) $(pfb2pfa_OBJECTS) $(pk2bm_OBJECTS) $(ps2pk_OBJECTS): $(KPATHSEA_DEPEND)

LDADD = $(KPATHSEA_LIBS)	

## Rebuild libkpathsea
@KPATHSEA_RULE@

dist_man1_MANS = mag.1 pfb2pfa.1 pk2bm.1 ps2pk.1

## Not used
##
EXTRA_DIST = \
	CHANGES.type1 \
	INSTALLATION \
	README.14m \
	README.type1 \
	basename.c \
	flisearch.c \
	pktest.c \
	protos.h \
	psearch.c \
	sexpr.c \
	strexpr.c \
	strexpr.h \
	t1test.c \
	texfiles.c \
	texfiles.h \
	usenet

