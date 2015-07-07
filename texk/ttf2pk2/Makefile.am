## Makefile.am for the TeX Live subdirectory texk/ttf2pk2/
##
## Copyright (C) 2011-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
AM_CPPFLAGS = -I$(top_srcdir)/include $(KPATHSEA_INCLUDES) $(FREETYPE2_INCLUDES)
AM_CPPFLAGS += -DHAVE_LIBKPATHSEA
AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = ttf2pk ttf2tfm

common_sources = \
	errormsg.c \
	errormsg.h \
	filesrch.c \
	filesrch.h \
	ftlib.c \
	ftlib.h \
	ligkern.c \
	ligkern.h \
	newobj.c \
	newobj.h \
	parse.c \
	parse.h \
	subfont.c \
	subfont.h \
	texenc.c \
	texenc.h \
	ttfenc.c \
	ttfenc.h

ttf2pk_SOURCES = \
	$(common_sources) \
	pklib.c \
	pklib.h \
	ttf2pk.c \
	ttflib.c \
	ttflib.h

ttf2tfm_SOURCES = \
	$(common_sources) \
	case.c \
	case.h \
	tfmaux.c \
	tfmaux.h \
	ttf2tfm.c \
	ttf2tfm.h \
	ttfaux.c \
	ttfaux.h \
	vplaux.c \
	vplaux.h

$(ttf2pk_OBJECTS) $(ttf2tfm_OBJECTS): config.force

config.force: $(KPATHSEA_DEPEND) $(FREETYPE2_DEPEND)
	echo timestamp >config.force
	$(SHELL) ./config.status --recheck

DISTCLEANFILES = config.force

LDADD = $(KPATHSEA_LIBS) $(FREETYPE2_LIBS) $(ZLIB_LIBS)

## Rebuild libkpathsea
@KPATHSEA_RULE@
## Rebuild libfreetype
@FREETYPE2_RULE@

dist_man1_MANS = ttf2pk.1 ttf2tfm.1

## Data
##
cfgdatadir = $(datarootdir)/texmf-dist/ttf2pk
dist_cfgdata_DATA = data/VPS.rpl data/ttf2pk.cfg

encdatadir = $(datarootdir)/texmf-dist/fonts/enc/ttf2pk/base
dist_encdata_DATA = data/T1-WGL4.enc

mapdatadir = $(datarootdir)/texmf-dist/source/fonts/zhmetrics
dist_mapdata_DATA = data/ttfonts.map

sfddatadir = $(datarootdir)/texmf-dist/fonts/sfd/ttf2pk
dist_sfddata_DATA = \
	data/Big5.sfd \
	data/EUC.sfd \
	data/HKSCS.sfd \
	data/KS-HLaTeX.sfd \
	data/SJIS.sfd \
	data/UBg5plus.sfd \
	data/UBig5.sfd \
	data/UGB.sfd \
	data/UGBK.sfd \
	data/UJIS.sfd \
	data/UKS-HLaTeX.sfd \
	data/UKS.sfd \
	data/Unicode.sfd

## Tests.
##
TESTS = tests/ttf2pk.test
tests/ttf2pk.log: ttf2pk$(EXEEXT) ttf2tfm$(EXEEXT)
EXTRA_DIST = $(TESTS)
## tests/ttf2pk.test
EXTRA_DIST += tests/GenR102.ttf tests/gentium-ec.enc tests/ttf2pk.cfg \
	tests/GenR102-ec.map tests/GenR102-h.map tests/GenR102-v.map \
	tests/GenR-ec.vpl tests/GenR102-ec.pl \
	tests/Test.sfd tests/ipaexg.ttf tests/ipaexg-h.map tests/ipaexg-h03.pl \
	tests/ipaexg-v.map tests/ipaexg-v03.pl
DISTCLEANFILES += GenR-ec.vpl GenR102-* ipaexg-*

## Not used
##
EXTRA_DIST += \
	BUGS \
	MakeSub.in \
	Makefile.dm \
	dvidrv.btm \
	dvidrv.doc \
	emdir.c \
	emdir.h \
	emtexdir.c \
	emtexdir.h \
	ttf2pk.doc

