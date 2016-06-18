## Proxy Makefile.am to build poppler for TeX Live.
##
##   Copyright (C) 2016 Karl Berry <tex-live@tug.org>
##   Copyright (C) 2011-2015 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
## We want to re-distribute the whole poppler source tree.
##
EXTRA_DIST = $(POPPLER_TREE)

## Changes applied to the original source tree
##
EXTRA_DIST += poppler-PATCHES

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

SUBDIRS = . include

GOO_SRC = $(top_srcdir)/$(POPPLER_TREE)/goo
FOFI_SRC = $(top_srcdir)/$(POPPLER_TREE)/fofi
POPPLER_SRC = $(top_srcdir)/$(POPPLER_TREE)/poppler

AM_CPPFLAGS = -I$(top_srcdir)/$(POPPLER_TREE) -I$(GOO_SRC) $(ZLIB_INCLUDES)
AM_CPPFLAGS += -DPOPPLER_DATADIR='"."'
AM_CFLAGS = $(VISIBILITY_CFLAGS) $(WARNING_CFLAGS)
AM_CXXFLAGS = $(VISIBILITY_CXXFLAGS) $(NO_WARN_CXXFLAGS)

noinst_LIBRARIES = libpoppler.a

$(libpoppler_a_OBJECTS): $(ZLIB_DEPEND)

nodist_libpoppler_a_SOURCES = \
	$(goo_libGoo_a_sources) \
	$(fofi_libfofi_a_sources) \
	$(poppler_libpoppler_a_sources)

goo_libGoo_a_sources = \
	@POPPLER_TREE@/goo/gfile.cc \
	@POPPLER_TREE@/goo/glibc.cc \
	@POPPLER_TREE@/goo/gmem.cc \
	@POPPLER_TREE@/goo/gmempp.cc \
	@POPPLER_TREE@/goo/grandom.cc \
	@POPPLER_TREE@/goo/gstrtod.cc \
	@POPPLER_TREE@/goo/FixedPoint.cc \
	@POPPLER_TREE@/goo/GooHash.cc \
	@POPPLER_TREE@/goo/GooList.cc \
	@POPPLER_TREE@/goo/GooString.cc \
	@POPPLER_TREE@/goo/GooTimer.cc

fofi_libfofi_a_sources = \
	@POPPLER_TREE@/fofi/FoFiBase.cc \
	@POPPLER_TREE@/fofi/FoFiEncodings.cc \
	@POPPLER_TREE@/fofi/FoFiIdentifier.cc \
	@POPPLER_TREE@/fofi/FoFiTrueType.cc \
	@POPPLER_TREE@/fofi/FoFiType1.cc \
	@POPPLER_TREE@/fofi/FoFiType1C.cc



poppler_libpoppler_a_sources = \
	@POPPLER_TREE@/poppler/Annot.cc \
	@POPPLER_TREE@/poppler/Array.cc \
	@POPPLER_TREE@/poppler/BuiltinFont.cc \
	@POPPLER_TREE@/poppler/BuiltinFontTables.cc \
	@POPPLER_TREE@/poppler/CMap.cc \
	@POPPLER_TREE@/poppler/CachedFile.cc \
	@POPPLER_TREE@/poppler/Catalog.cc \
	@POPPLER_TREE@/poppler/CharCodeToUnicode.cc \
	@POPPLER_TREE@/poppler/DateInfo.cc \
	@POPPLER_TREE@/poppler/Decrypt.cc \
	@POPPLER_TREE@/poppler/Dict.cc \
	@POPPLER_TREE@/poppler/Error.cc \
	@POPPLER_TREE@/poppler/FileSpec.cc \
	@POPPLER_TREE@/poppler/FlateStream.cc \
	@POPPLER_TREE@/poppler/FontEncodingTables.cc \
	@POPPLER_TREE@/poppler/FontInfo.cc \
	@POPPLER_TREE@/poppler/Form.cc \
	@POPPLER_TREE@/poppler/Function.cc \
	@POPPLER_TREE@/poppler/Gfx.cc \
	@POPPLER_TREE@/poppler/GfxFont.cc \
	@POPPLER_TREE@/poppler/GfxState.cc \
	@POPPLER_TREE@/poppler/GlobalParams.cc \
	@POPPLER_TREE@/poppler/Hints.cc \
	@POPPLER_TREE@/poppler/JArithmeticDecoder.cc \
	@POPPLER_TREE@/poppler/JBIG2Stream.cc \
	@POPPLER_TREE@/poppler/JPXStream.cc \
	@POPPLER_TREE@/poppler/Lexer.cc \
	@POPPLER_TREE@/poppler/Linearization.cc \
	@POPPLER_TREE@/poppler/Link.cc \
	@POPPLER_TREE@/poppler/MarkedContentOutputDev.cc \
	@POPPLER_TREE@/poppler/Movie.cc \
	@POPPLER_TREE@/poppler/NameToCharCode.cc \
	@POPPLER_TREE@/poppler/Object.cc \
	@POPPLER_TREE@/poppler/OptionalContent.cc \
	@POPPLER_TREE@/poppler/Outline.cc \
	@POPPLER_TREE@/poppler/OutputDev.cc \
	@POPPLER_TREE@/poppler/PDFDoc.cc \
	@POPPLER_TREE@/poppler/PDFDocEncoding.cc \
	@POPPLER_TREE@/poppler/PDFDocFactory.cc \
	@POPPLER_TREE@/poppler/PSTokenizer.cc \
	@POPPLER_TREE@/poppler/Page.cc \
	@POPPLER_TREE@/poppler/PageLabelInfo.cc \
	@POPPLER_TREE@/poppler/PageTransition.cc \
	@POPPLER_TREE@/poppler/Parser.cc \
	@POPPLER_TREE@/poppler/PopplerCache.cc \
	@POPPLER_TREE@/poppler/PreScanOutputDev.cc \
	@POPPLER_TREE@/poppler/ProfileData.cc \
	@POPPLER_TREE@/poppler/Rendition.cc \
	@POPPLER_TREE@/poppler/SecurityHandler.cc \
	@POPPLER_TREE@/poppler/SignatureInfo.cc \
	@POPPLER_TREE@/poppler/Sound.cc \
	@POPPLER_TREE@/poppler/StdinCachedFile.cc \
	@POPPLER_TREE@/poppler/StdinPDFDocBuilder.cc \
	@POPPLER_TREE@/poppler/Stream.cc \
	@POPPLER_TREE@/poppler/StructElement.cc \
	@POPPLER_TREE@/poppler/StructTreeRoot.cc \
	@POPPLER_TREE@/poppler/UTF.cc \
	@POPPLER_TREE@/poppler/UnicodeMap.cc \
	@POPPLER_TREE@/poppler/UnicodeTypeTable.cc \
	@POPPLER_TREE@/poppler/ViewerPreferences.cc \
	@POPPLER_TREE@/poppler/XRef.cc \
	@POPPLER_TREE@/poppler/XpdfPluginAPI.cc \
	@POPPLER_TREE@/poppler/strtok_r.cpp

$(libpoppler_a_OBJECTS): config.force

## Rebuild zlib
@ZLIB_RULE@

# Reconfig
reconfig_prereq = $(ZLIB_DEPEND)
DISTCLEANFILES =

include $(srcdir)/../../am/reconfig.am

# Rebuild
rebuild_prereq =
rebuild_target = all
CLEANFILES =

include $(srcdir)/../../am/rebuild.am

