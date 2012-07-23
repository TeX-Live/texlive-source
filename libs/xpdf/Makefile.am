## Proxy Makefile.am to build xpdf for TeX Live.
##
##   Copyright (C) 2009-2012 Peter Breitenlohner <tex-live@tug.org>
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
	rm -rf `find $(distdir) -name .svn`
	rm -f $(distdir)/$(XPDF_TREE)/install-sh

SUBDIRS = goo fofi splash xpdf

GOO_SRC = $(top_srcdir)/$(XPDF_TREE)/goo
FOFI_SRC = $(top_srcdir)/$(XPDF_TREE)/fofi
SPLAHS_SRC = $(top_srcdir)/$(XPDF_TREE)/splash
XPDF_SRC = $(top_srcdir)/$(XPDF_TREE)/xpdf

INCLUDES = -Igoo -Ifofi -Isplash -Ixpdf -I$(GOO_SRC) -I$(FOFI_SRC) -I$(SPLAHS_SRC) -I$(XPDF_SRC)
AM_CPPFLAGS = -DPDF_PARSER_ONLY
AM_CFLAGS = $(WARNING_CFLAGS)
AM_CXXFLAGS = $(NO_WARN_CXXFLAGS)

noinst_LIBRARIES = libxpdf.a

nodist_libxpdf_a_SOURCES = \
	$(goo_libGoo_a_sources) \
	$(fofi_libfofi_a_sources) \
	$(xpdf_libxpdf_a_sources)

goo_libGoo_a_sources = \
	$(GOO_SRC)/FixedPoint.cc \
	$(GOO_SRC)/GHash.cc \
	$(GOO_SRC)/GList.cc \
	$(GOO_SRC)/GString.cc \
	$(GOO_SRC)/gmem.cc \
	$(GOO_SRC)/gmempp.cc \
	$(GOO_SRC)/gfile.cc

fofi_libfofi_a_sources = \
	$(FOFI_SRC)/FoFiBase.cc \
	$(FOFI_SRC)/FoFiEncodings.cc \
	$(FOFI_SRC)/FoFiIdentifier.cc \
	$(FOFI_SRC)/FoFiTrueType.cc \
	$(FOFI_SRC)/FoFiType1.cc \
	$(FOFI_SRC)/FoFiType1C.cc

xpdf_libxpdf_a_sources = \
	$(XPDF_SRC)/Array.cc \
	$(XPDF_SRC)/BuiltinFont.cc \
	$(XPDF_SRC)/BuiltinFontTables.cc \
	$(XPDF_SRC)/CMap.cc \
	$(XPDF_SRC)/Catalog.cc \
	$(XPDF_SRC)/CharCodeToUnicode.cc \
	$(XPDF_SRC)/Decrypt.cc \
	$(XPDF_SRC)/Dict.cc \
	$(XPDF_SRC)/Error.cc \
	$(XPDF_SRC)/FontEncodingTables.cc \
	$(XPDF_SRC)/Function.cc \
	$(XPDF_SRC)/GfxFont.cc \
	$(XPDF_SRC)/GfxState.cc \
	$(XPDF_SRC)/GlobalParams.cc \
	$(XPDF_SRC)/JArithmeticDecoder.cc \
	$(XPDF_SRC)/JBIG2Stream.cc \
	$(XPDF_SRC)/JPXStream.cc \
	$(XPDF_SRC)/Lexer.cc \
	$(XPDF_SRC)/Link.cc \
	$(XPDF_SRC)/NameToCharCode.cc \
	$(XPDF_SRC)/Object.cc \
	$(XPDF_SRC)/OptionalContent.cc \
	$(XPDF_SRC)/Outline.cc \
	$(XPDF_SRC)/OutputDev.cc \
	$(XPDF_SRC)/PDFDoc.cc \
	$(XPDF_SRC)/PDFDocEncoding.cc \
	$(XPDF_SRC)/PSTokenizer.cc \
	$(XPDF_SRC)/Page.cc \
	$(XPDF_SRC)/Parser.cc \
	$(XPDF_SRC)/SecurityHandler.cc \
	$(XPDF_SRC)/Stream.cc \
	$(XPDF_SRC)/UnicodeMap.cc \
	$(XPDF_SRC)/XRef.cc

