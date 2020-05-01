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
	tests/uni.idx tests/uni.dict tests/uni.ist tests/uni.ind
DISTCLEANFILES = foo.* uni.*

