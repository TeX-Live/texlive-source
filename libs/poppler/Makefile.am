## Proxy Makefile.am to build poppler for TeX Live.
##
##   Copyright (C) 2011, 2012 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
ACLOCAL_AMFLAGS = -I ../../m4

# Rebuild
.PHONY: rebuild
rebuild: all

## We want to re-distribute the whole original poppler source tree.
##
EXTRA_DIST = $(POPPLER_TREE)

## Patches applied to the original source tree
##
EXTRA_DIST += $(POPPLER_TREE)-PATCHES

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`

SUBDIRS = goo fofi poppler

GOO_SRC = $(top_srcdir)/$(POPPLER_TREE)/goo
FOFI_SRC = $(top_srcdir)/$(POPPLER_TREE)/fofi
POPPLER_SRC = $(top_srcdir)/$(POPPLER_TREE)/poppler

INCLUDES = -Igoo -Ifofi -Ipoppler -I$(GOO_SRC) -I$(FOFI_SRC) -I$(POPPLER_SRC) $(ZLIB_INCLUDES)
AM_CPPFLAGS = -DPOPPLER_DATADIR='"."'
AM_CFLAGS = $(WARNING_CFLAGS)
AM_CXXFLAGS = $(NO_WARN_CXXFLAGS)

noinst_LIBRARIES = libpoppler.a

nodist_libpoppler_a_SOURCES = \
	$(goo_libGoo_a_sources) \
	$(fofi_libfofi_a_sources) \
	$(poppler_libpoppler_a_sources)

goo_libGoo_a_sources = \
	$(GOO_SRC)/FixedPoint.cc \
	$(GOO_SRC)/gfile.cc \
	$(GOO_SRC)/gmem.cc \
	$(GOO_SRC)/gmempp.cc \
	$(GOO_SRC)/GooHash.cc \
	$(GOO_SRC)/GooList.cc \
	$(GOO_SRC)/GooString.cc \
	$(GOO_SRC)/GooTimer.cc \
	$(GOO_SRC)/grandom.cc \
	$(GOO_SRC)/gstrtod.cc

fofi_libfofi_a_sources = \
	$(FOFI_SRC)/FoFiBase.cc \
	$(FOFI_SRC)/FoFiEncodings.cc \
	$(FOFI_SRC)/FoFiIdentifier.cc \
	$(FOFI_SRC)/FoFiTrueType.cc \
	$(FOFI_SRC)/FoFiType1.cc \
	$(FOFI_SRC)/FoFiType1C.cc

poppler_libpoppler_a_sources = \
	$(POPPLER_SRC)/Annot.cc \
	$(POPPLER_SRC)/Array.cc \
	$(POPPLER_SRC)/BuiltinFont.cc \
	$(POPPLER_SRC)/BuiltinFontTables.cc \
	$(POPPLER_SRC)/CMap.cc \
	$(POPPLER_SRC)/CachedFile.cc \
	$(POPPLER_SRC)/Catalog.cc \
	$(POPPLER_SRC)/CharCodeToUnicode.cc \
	$(POPPLER_SRC)/DateInfo.cc \
	$(POPPLER_SRC)/Decrypt.cc \
	$(POPPLER_SRC)/Dict.cc \
	$(POPPLER_SRC)/Error.cc \
	$(POPPLER_SRC)/FileSpec.cc \
	$(POPPLER_SRC)/FlateStream.cc \
	$(POPPLER_SRC)/FontEncodingTables.cc \
	$(POPPLER_SRC)/FontInfo.cc \
	$(POPPLER_SRC)/Form.cc \
	$(POPPLER_SRC)/Function.cc \
	$(POPPLER_SRC)/Gfx.cc \
	$(POPPLER_SRC)/GfxFont.cc \
	$(POPPLER_SRC)/GfxState.cc \
	$(POPPLER_SRC)/GlobalParams.cc \
	$(POPPLER_SRC)/Hints.cc \
	$(POPPLER_SRC)/JArithmeticDecoder.cc \
	$(POPPLER_SRC)/JBIG2Stream.cc \
	$(POPPLER_SRC)/JPXStream.cc \
	$(POPPLER_SRC)/Linearization.cc \
	$(POPPLER_SRC)/Lexer.cc \
	$(POPPLER_SRC)/Link.cc \
	$(POPPLER_SRC)/Movie.cc \
	$(POPPLER_SRC)/NameToCharCode.cc \
	$(POPPLER_SRC)/Object.cc \
	$(POPPLER_SRC)/OptionalContent.cc \
	$(POPPLER_SRC)/Outline.cc \
	$(POPPLER_SRC)/OutputDev.cc \
	$(POPPLER_SRC)/PDFDoc.cc \
	$(POPPLER_SRC)/PDFDocEncoding.cc \
	$(POPPLER_SRC)/PSTokenizer.cc \
	$(POPPLER_SRC)/Page.cc \
	$(POPPLER_SRC)/PageLabelInfo.cc \
	$(POPPLER_SRC)/PageTransition.cc \
	$(POPPLER_SRC)/Parser.cc \
	$(POPPLER_SRC)/PopplerCache.cc \
	$(POPPLER_SRC)/PreScanOutputDev.cc \
	$(POPPLER_SRC)/ProfileData.cc \
	$(POPPLER_SRC)/Rendition.cc \
	$(POPPLER_SRC)/SecurityHandler.cc \
	$(POPPLER_SRC)/Sound.cc \
	$(POPPLER_SRC)/Stream.cc \
	$(POPPLER_SRC)/strtok_r.cpp \
	$(POPPLER_SRC)/UnicodeMap.cc \
	$(POPPLER_SRC)/UnicodeTypeTable.cc \
	$(POPPLER_SRC)/UTF.cc \
	$(POPPLER_SRC)/ViewerPreferences.cc \
	$(POPPLER_SRC)/XRef.cc \
	$(POPPLER_SRC)/XpdfPluginAPI.cc

$(libpoppler_a_OBJECTS): config.force

config.force: $(ZLIB_DEPEND)
	echo timestamp >config.force
	$(SHELL) ./config.status --recheck

DISTCLEANFILES = config.force

## Rebuild zlib
@ZLIB_RULE@

