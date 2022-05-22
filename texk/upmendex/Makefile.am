## $Id$
## Makefile.am for the TeX Live subdirectory texk/upmendex/
##
## Copyright 2015-2019 Karl Berry <tex-live@tug.org>
## Copyright 2010-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
AM_CPPFLAGS = $(KPATHSEA_INCLUDES) $(ICU_INCLUDES)
AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = upmendex

upmendex_SOURCES = \
	convert.c \
	exkana.h \
	exvar.h \
	fread.c \
	fwrite.c \
	kana.h \
	kp.c \
	kp.h \
	main.c \
	mendex.h \
	version.h \
	pageread.c \
	qsort.c \
	qsort.h \
	sort.c \
	styfile.c \
	var.h

# Force Automake to use CXXLD for linking
nodist_EXTRA_upmendex_SOURCES = dummy.cxx

$(upmendex_OBJECTS): $(KPATHSEA_DEPEND) $(ICU_DEPEND)

LDADD = $(KPATHSEA_LIBS) $(ICU_LIBS) $(PTHREAD_LIB) $(ICU_LIBS_EXTRA)

## Rebuild libkpathsea
@KPATHSEA_RULE@
## Rebuild libsicuxxx
@ICU_RULE@

EXTRA_DIST = \
	COPYRIGHT

## Eventually delete these files
##
EXTRA_DIST += \
	indexsty \
	upmendex.ja.txt

## Tests
##
TESTS = tests/upmendex.test
EXTRA_DIST += $(TESTS)

## tests/upmendex.test
EXTRA_DIST += tests/foo.idx tests/foo.ind \
	tests/uni.idx tests/uni.dict tests/uni.ist \
	tests/uni1.ist tests/uni2.ist tests/uni.ind \
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
EXTRA_DIST += tests/locale1.ist tests/locale2.ist tests/rule.idx \
	tests/attr1.ist tests/attr1.ind tests/locale1.ind tests/locale2-rule.ind \
	tests/rule1.ist tests/rule2.ist tests/rule3.ist tests/rule4.ist rule.ind
DISTCLEANFILES += locale*.* attr*.* rule*.*
