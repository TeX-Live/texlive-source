## Proxy Makefile.am to build xpdf for TeX Live.
##
##   Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
#========================================================================
##
## Main xpdf Makefile.
## Goo library Makefile
## FoFi library Makefile
## Xpdf Makefile
##
## Copyright 1996-2003 Glyph & Cog, LLC
##
#========================================================================
##
ACLOCAL_AMFLAGS = -I ../../m4

# Rebuild
.PHONY: rebuild
rebuild: all

## We want to re-distribute the whole original xpdf source tree.
##
## With current automake (1.10.2) 'make distcheck' fails when
## DISTFILES contains a directory and files in that directory.
## Thus nodist_* for all files in $(XPDF_TREE).
EXTRA_DIST = $(XPDF_TREE)

## Official patches as obtained from ftp://ftp.foolabs.com/pub/xpdf
## (if any)
##
EXTRA_DIST += $(XPDF_TREE)-foolabs

## Actual patches applied to the original source tree
##
EXTRA_DIST += $(XPDF_TREE)-PATCHES

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn -o -name .deps -o -name .dirstamp -o -name '*.$(OBJEXT)'`
	rm -f $(distdir)/$(XPDF_TREE)/install-sh

SUBDIRS = goo fofi splash xpdf

GOO_SRC = $(top_srcdir)/$(XPDF_TREE)/goo
FOFI_SRC = $(top_srcdir)/$(XPDF_TREE)/fofi
SPLASH_SRC = $(top_srcdir)/$(XPDF_TREE)/splash
XPDF_SRC = $(top_srcdir)/$(XPDF_TREE)/xpdf

AM_CPPFLAGS = -Igoo -Ifofi -Isplash -Ixpdf
AM_CPPFLAGS += -I$(FOFI_SRC) -I$(SPLASH_SRC) -I$(XPDF_SRC)
AM_CPPFLAGS += -DPDF_PARSER_ONLY
AM_CFLAGS = $(WARNING_CFLAGS)
AM_CXXFLAGS = $(NO_WARN_CXXFLAGS)

noinst_LIBRARIES = libxpdf.a

nodist_libxpdf_a_SOURCES = \
	$(goo_libGoo_a_sources) \
	$(fofi_libfofi_a_sources) \
	$(xpdf_libxpdf_a_sources)

goo_libGoo_a_sources = \
	@XPDF_TREE@/goo/FixedPoint.cc \
	@XPDF_TREE@/goo/GHash.cc \
	@XPDF_TREE@/goo/GList.cc \
	@XPDF_TREE@/goo/GString.cc \
	@XPDF_TREE@/goo/gmem.cc \
	@XPDF_TREE@/goo/gmempp.cc \
	@XPDF_TREE@/goo/gfile.cc

fofi_libfofi_a_sources = \
	@XPDF_TREE@/fofi/FoFiBase.cc \
	@XPDF_TREE@/fofi/FoFiEncodings.cc \
	@XPDF_TREE@/fofi/FoFiIdentifier.cc \
	@XPDF_TREE@/fofi/FoFiTrueType.cc \
	@XPDF_TREE@/fofi/FoFiType1.cc \
	@XPDF_TREE@/fofi/FoFiType1C.cc

xpdf_libxpdf_a_sources = \
	@XPDF_TREE@/xpdf/Array.cc \
	@XPDF_TREE@/xpdf/BuiltinFont.cc \
	@XPDF_TREE@/xpdf/BuiltinFontTables.cc \
	@XPDF_TREE@/xpdf/CMap.cc \
	@XPDF_TREE@/xpdf/Catalog.cc \
	@XPDF_TREE@/xpdf/CharCodeToUnicode.cc \
	@XPDF_TREE@/xpdf/Decrypt.cc \
	@XPDF_TREE@/xpdf/Dict.cc \
	@XPDF_TREE@/xpdf/Error.cc \
	@XPDF_TREE@/xpdf/FontEncodingTables.cc \
	@XPDF_TREE@/xpdf/Function.cc \
	@XPDF_TREE@/xpdf/GfxFont.cc \
	@XPDF_TREE@/xpdf/GfxState.cc \
	@XPDF_TREE@/xpdf/GlobalParams.cc \
	@XPDF_TREE@/xpdf/JArithmeticDecoder.cc \
	@XPDF_TREE@/xpdf/JBIG2Stream.cc \
	@XPDF_TREE@/xpdf/JPXStream.cc \
	@XPDF_TREE@/xpdf/Lexer.cc \
	@XPDF_TREE@/xpdf/Link.cc \
	@XPDF_TREE@/xpdf/NameToCharCode.cc \
	@XPDF_TREE@/xpdf/Object.cc \
	@XPDF_TREE@/xpdf/OptionalContent.cc \
	@XPDF_TREE@/xpdf/Outline.cc \
	@XPDF_TREE@/xpdf/OutputDev.cc \
	@XPDF_TREE@/xpdf/PDFDoc.cc \
	@XPDF_TREE@/xpdf/PDFDocEncoding.cc \
	@XPDF_TREE@/xpdf/PSTokenizer.cc \
	@XPDF_TREE@/xpdf/Page.cc \
	@XPDF_TREE@/xpdf/Parser.cc \
	@XPDF_TREE@/xpdf/SecurityHandler.cc \
	@XPDF_TREE@/xpdf/Stream.cc \
	@XPDF_TREE@/xpdf/UnicodeMap.cc \
	@XPDF_TREE@/xpdf/XRef.cc

