## Makefile.am for the TeX Live subdirectory texk/ptexenc/
##
## Copyright (C) 2010-2014 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
AM_CPPFLAGS = $(KPATHSEA_INCLUDES)
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

# Rebuild
rebuild_prereq =
rebuild_target = all
CLEANFILES =

include $(srcdir)/../../am/rebuild.am

