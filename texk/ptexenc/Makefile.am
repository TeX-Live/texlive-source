## Makefile.am for the TeX Live subdirectory texk/ptexenc/
##
## Copyright (C) 2010-2014 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
AM_CPPFLAGS = $(PTEXENC_INCLUDES) $(KPATHSEA_INCLUDES) -DKPATHSEA -DPTEXENC
AM_CFLAGS = $(WARNING_CFLAGS)

lib_LTLIBRARIES = libptexenc.la

libptexenc_la_CPPFLAGS = $(AM_CPPFLAGS) -DMAKE_PTENC_DLL

libptexenc_la_LDFLAGS = -bindir @bindir@ -no-undefined -version-info $(PTEXENC_LT_VERSINFO)

# Note: jisx0208.h is a copy of ../../libs/gd/gd-2.0.35/jisx0208.h.
#
libptexenc_la_SOURCES = \
	jisx0208.h \
	kanjicnv.c \
	ptexenc.c \
	ptexenc/kanjicnv.h \
	ptexenc/unicode-jp.h \
	unicode-jp.c \
	unicode.c

$(libptexenc_la_OBJECTS): $(KPATHSEA_DEPEND)

libptexenc_la_LIBADD = $(KPATHSEA_LIBS)

## Rebuild libkpathsea
@KPATHSEA_RULE@

nobase_include_HEADERS = \
	ptexenc/ptexenc.h \
	ptexenc/unicode.h

pkgconfigdir = ${libdir}/pkgconfig

pkgconfig_DATA = ptexenc.pc

EXTRA_DIST = COPYRIGHT ChangeLog.jp

## The programs
bin_PROGRAMS = ptekf
ptekf_LDADD = libptexenc.la

LDADD = $(KPATHSEA_LIBS)

## Tests
#
TESTS = tests/ptekf-smoke.test tests/ptekf-guess.test  tests/ptekf-conv.test
#
tests/ptekf-smoke.log tests/ptekf-guess.log tests/ptekf-conv.log \
  : ptekf$(EXEEXT)
#
EXTRA_DIST += $(TESTS)
#
## ptekf-conv.test
EXTRA_DIST += \
  tests/enc-amb0.bib-euc tests/enc-amb0.bib-euc-utf8 tests/enc-amb0.bib-jis \
  tests/enc-amb0.bib-sjis tests/enc-amb0.bib-sjis-utf8 tests/enc-amb0.bib-utf8 \
  tests/enc-amb1.bib-euc tests/enc-amb1.bib-jis tests/enc-amb1.bib-sjis \
  tests/enc-amb1.bib-sjis-utf8 tests/enc-amb1.bib-utf8 \
  tests/enc-amb2.bib-euc-utf8 tests/enc-amb2.bib-sjis-utf8 \
  tests/enc-euc.bib-utf8 tests/enc-jis.bib-utf8 tests/enc-sjis.bib-utf8 \
  tests/enc-utf8.bib-euc tests/enc-utf8.bib-jis tests/enc-utf8.bib-sjis tests/enc-utf8.bib-utf8 \
  tests/enc-utf8a.bib-euc tests/enc-utf8a.bib-jis tests/enc-utf8a.bib-sjis tests/enc-utf8a.bib-utf8 \
  tests/enc-utf8b.bib-euc tests/enc-utf8b.bib-jis tests/enc-utf8b.bib-sjis tests/enc-utf8b.bib-utf8
DISTCLEANFILES = enc-*.bib*


# Rebuild
rebuild_prereq =
rebuild_target = all
CLEANFILES =

include $(srcdir)/../../am/rebuild.am

