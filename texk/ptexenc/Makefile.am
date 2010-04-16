## Makefile.am for the TeX Live subdirectory texk/ptexenc/
##
## Copyright (C) 2010 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

# Rebuild
.PHONY: rebuild
rebuild: all

INCLUDES = $(KPATHSEA_INCLUDES) -I$(top_builddir)/.. -I$(top_srcdir)/..
AM_CFLAGS = $(WARNING_CFLAGS)

lib_LTLIBRARIES = libptexenc.la

libptexenc_la_CPPFLAGS = -DMAKE_KPSE_DLL

libptexenc_la_LDFLAGS = -version-info $(PTEXENC_LT_VERSINFO)

libptexenc_la_SOURCES = kanjicnv.c ptexenc.c unicode-jp.c unicode.c

## Rebuild libkpathsea
@KPATHSEA_RULE@

ptexencincludedir = ${includedir}/ptexenc

nodist_ptexencinclude_HEADERS = c-auto.h

ptexencinclude_HEADERS = kanjicnv.h ptexenc.h unicode-jp.h unicode.h

EXTRA_DIST = COPYRIGHT ChangeLog.TL

## Eventually delete these files
##
EXTRA_DIST += \
	patches/jmpost-0.04b-ptexenc.patch \
	patches/makejvf-1.1a-ptexenc.patch \
	patches/mendexk2.6f-ptexenc.patch \
	patches/ptex-src-3.1.11-ptexenc.patch \
	patches/texlive2009-ptexenc.patch \
	patches/texlive2009.sh
