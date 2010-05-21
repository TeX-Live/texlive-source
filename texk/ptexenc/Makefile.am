## Makefile.am for the TeX Live subdirectory texk/ptexenc/
##
## Copyright (C) 2010 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

# Rebuild
.PHONY: rebuild
rebuild: all

INCLUDES = $(KPATHSEA_INCLUDES)
AM_CFLAGS = $(WARNING_CFLAGS)

lib_LTLIBRARIES = libptexenc.la

libptexenc_la_CPPFLAGS = -DMAKE_KPSE_DLL

libptexenc_la_LDFLAGS = -version-info $(PTEXENC_LT_VERSINFO)

# Note: jisx0208.h is a copy of ../../libs/gd/gd-2.0.35/jisx0208.h.
#
libptexenc_la_SOURCES = \
	jisx0208.h \
	kanjicnv.c \
	ptexenc.c \
	unicode-jp.c \
	unicode.c

## Rebuild libkpathsea
@KPATHSEA_RULE@

nobase_nodist_include_HEADERS = \
	ptexenc/c-auto.h

nobase_include_HEADERS = \
	ptexenc/kanjicnv.h \
	ptexenc/ptexenc.h \
	ptexenc/unicode-jp.h \
	ptexenc/unicode.h

EXTRA_DIST = COPYRIGHT ChangeLog.TL

