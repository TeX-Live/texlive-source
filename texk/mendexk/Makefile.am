## Makefile.am for the TeX Live subdirectory texk/mendexk/
##
## Copyright (C) 2010-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
AM_CPPFLAGS = $(PTEXENC_INCLUDES) $(KPATHSEA_INCLUDES)
AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = mendex

mendex_SOURCES = \
	convert.c \
	exkana.h \
	exvar.h \
	fread.c \
	fwrite.c \
	kana.h \
	kp.c \
	kp.h \
	ktable.h \
	main.c \
	mendex.h \
	version.h \
	pageread.c \
	qsort.c \
	qsort.h \
	sort.c \
	styfile.c \
	var.h

$(mendex_OBJECTS): $(KPATHSEA_DEPEND) $(PTEXENC_DEPEND)

LDADD = $(PTEXENC_LIBS) $(KPATHSEA_LIBS)

## Rebuild libkpathsea & libptexenc
@KPATHSEA_RULE@
@PTEXENC_RULE@

dist_man1_MANS = mendex.1

EXTRA_DIST = \
	COPYRIGHT \
	COPYRIGHT.ja \
	ChangeLog.ja

## Eventually delete these files
##
EXTRA_DIST += \
	indexsty \
	mendex.ja.txt mendex.1 mendex.1.ja

## Tests
##
TESTS = tests/mendex.test
tests/mendex.log: mendex$(EXEEXT)
EXTRA_DIST += $(TESTS)

## tests/mendex.test
EXTRA_DIST += tests/foo.idx tests/foo.ind \
	tests/uni.idx tests/uni.dict tests/uni.ist \
	tests/uni1.ist tests/uni2.ist tests/uni.ind \
	tests/sjis.ist tests/euc.ist \
	tests/range.idx tests/range2.ist tests/range2.ist tests/range3.ist \
	tests/range1.ind tests/range2.ind tests/range3.ind \
	tests/pprec0.ist tests/pprec1.ist tests/pprec2.ist tests/pprec3.ist \
	tests/pprec4.ist tests/pprec5.ist tests/pprec6.ist tests/pprec7.ist \
	tests/pprecA.idx tests/pprecA-0.ind tests/pprecA-1.ind tests/pprecA-2.ind \
	tests/pprecB.idx tests/pprecB-0.ind tests/pprecB-3.ind tests/pprecB-4.ind \
	tests/romalpA.idx tests/romalpA-5.ind tests/romalpA-6.ind \
	tests/romalpB.idx tests/romalpB-5.ind tests/romalpB-6.ind tests/romalpB-7.ind \
	tests/romalpC.idx tests/romalpC-5.ind \
	tests/romalpD.idx tests/romalpD-5.ind tests/romalpD-6.ind tests/romalpD-7.ind
DISTCLEANFILES = foo.* uni.* range*.* pprec*.* romalp*.*

