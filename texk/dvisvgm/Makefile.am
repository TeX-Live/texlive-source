## Makefile.am for the TeX Live subdirectory texk/dvisvgm/
##
## Copyright (C) 2009-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
#************************************************************************
#
#  Adapted for TeX Live from dvisvgm-0.8.7/Makefile.am
#  Copyright (C) 2005-2009 Martin Gieseking <martin.gieseking@uos.de>
#
#************************************************************************
##
## We want to re-distribute the whole original dvisvgm source tree.
##
EXTRA_DIST = $(DVISVGM_TREE)

## Patches applied to the original source tree
##
EXTRA_DIST += $(DVISVGM_TREE)-PATCHES

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

AM_CPPFLAGS = -I$(top_srcdir)/$(DVISVGM_TREE)/clipper -I$(top_srcdir)/$(DVISVGM_TREE)/src
AM_CPPFLAGS += $(KPATHSEA_INCLUDES) $(POTRACE_INCLUDES) $(FREETYPE2_INCLUDES)
AM_CPPFLAGS += $(ZLIB_INCLUDES) $(LIBGS_INCLUDES)
if WIN32
AM_CPPFLAGS += -DTEXLIVEWIN32
endif WIN32
AM_CFLAGS = $(WARNING_CFLAGS)
AM_CXXFLAGS = $(WARNING_CXXFLAGS)

nodist_man_MANS = \
	$(DVISVGM_TREE)/doc/dvisvgm.1

bin_PROGRAMS = dvisvgm

nodist_dvisvgm_SOURCES = \
	@DVISVGM_TREE@/src/dvisvgm.cpp \
	@DVISVGM_TREE@/src/gzstream.cpp

dvisvgm_dependencies = $(KPATHSEA_DEPEND) $(ZLIB_DEPEND) $(FREETYPE2_DEPEND)

$(dvisvgm_OBJECTS): $(noinst_LIBRARIES)

LDADD = $(noinst_LIBRARIES) $(KPATHSEA_LIBS) $(POTRACE_LIBS) $(FREETYPE2_LIBS)
LDADD += $(ZLIB_LIBS) $(LIBGS_LIBS)

noinst_LIBRARIES = libdvisvgm.a libclipper.a

nodist_libdvisvgm_a_SOURCES = \
	@DVISVGM_TREE@/src/BasicDVIReader.cpp \
	@DVISVGM_TREE@/src/Bezier.cpp \
	@DVISVGM_TREE@/src/BgColorSpecialHandler.cpp \
	@DVISVGM_TREE@/src/Bitmap.cpp \
	@DVISVGM_TREE@/src/BoundingBox.cpp \
	@DVISVGM_TREE@/src/CMap.cpp \
	@DVISVGM_TREE@/src/CMapManager.cpp \
	@DVISVGM_TREE@/src/CMapReader.cpp \
	@DVISVGM_TREE@/src/CRC32.cpp \
	@DVISVGM_TREE@/src/Calculator.cpp \
	@DVISVGM_TREE@/src/CharMapID.cpp \
	@DVISVGM_TREE@/src/CmdLineParserBase.cpp \
	@DVISVGM_TREE@/src/Color.cpp \
	@DVISVGM_TREE@/src/ColorSpecialHandler.cpp \
	@DVISVGM_TREE@/src/CommandLine.cpp \
	@DVISVGM_TREE@/src/DVIReader.cpp \
	@DVISVGM_TREE@/src/DVIToSVG.cpp \
	@DVISVGM_TREE@/src/DVIToSVGActions.cpp \
	@DVISVGM_TREE@/src/DependencyGraph.h \
	@DVISVGM_TREE@/src/Directory.cpp \
	@DVISVGM_TREE@/src/DvisvgmSpecialHandler.cpp \
	@DVISVGM_TREE@/src/EPSFile.cpp \
	@DVISVGM_TREE@/src/EPSToSVG.cpp \
	@DVISVGM_TREE@/src/EmSpecialHandler.cpp \
	@DVISVGM_TREE@/src/EncFile.cpp \
	@DVISVGM_TREE@/src/FileFinder.cpp \
	@DVISVGM_TREE@/src/FilePath.cpp \
	@DVISVGM_TREE@/src/FileSystem.cpp \
	@DVISVGM_TREE@/src/Font.cpp \
	@DVISVGM_TREE@/src/FontCache.cpp \
	@DVISVGM_TREE@/src/FontEncoding.cpp \
	@DVISVGM_TREE@/src/FontEngine.cpp \
	@DVISVGM_TREE@/src/FontManager.cpp \
	@DVISVGM_TREE@/src/FontMap.cpp \
	@DVISVGM_TREE@/src/FontMetrics.cpp \
	@DVISVGM_TREE@/src/GFReader.cpp \
	@DVISVGM_TREE@/src/GFGlyphTracer.cpp \
	@DVISVGM_TREE@/src/GFTracer.cpp \
	@DVISVGM_TREE@/src/Ghostscript.cpp \
	@DVISVGM_TREE@/src/HtmlSpecialHandler.cpp \
	@DVISVGM_TREE@/src/InputBuffer.cpp \
	@DVISVGM_TREE@/src/InputReader.cpp \
	@DVISVGM_TREE@/src/JFM.cpp \
	@DVISVGM_TREE@/src/Length.cpp \
	@DVISVGM_TREE@/src/MapLine.cpp \
	@DVISVGM_TREE@/src/Matrix.cpp \
	@DVISVGM_TREE@/src/Message.cpp \
	@DVISVGM_TREE@/src/MetafontWrapper.cpp \
	@DVISVGM_TREE@/src/NoPsSpecialHandler.cpp \
	@DVISVGM_TREE@/src/PSInterpreter.cpp \
	@DVISVGM_TREE@/src/PSPattern.cpp \
	@DVISVGM_TREE@/src/PSPreviewFilter.cpp \
	@DVISVGM_TREE@/src/PdfSpecialHandler.cpp \
	@DVISVGM_TREE@/src/PageRanges.cpp \
	@DVISVGM_TREE@/src/PageSize.cpp \
	@DVISVGM_TREE@/src/PathClipper.cpp \
	@DVISVGM_TREE@/src/PreScanDVIReader.cpp \
	@DVISVGM_TREE@/src/Process.cpp \
	@DVISVGM_TREE@/src/PsSpecialHandler.cpp \
	@DVISVGM_TREE@/src/psdefs.cpp \
	@DVISVGM_TREE@/src/RangeMap.cpp \
	@DVISVGM_TREE@/src/SVGOutput.cpp \
	@DVISVGM_TREE@/src/SVGTree.cpp \
	@DVISVGM_TREE@/src/ShadingPatch.cpp \
	@DVISVGM_TREE@/src/SignalHandler.cpp \
	@DVISVGM_TREE@/src/SpecialManager.cpp \
	@DVISVGM_TREE@/src/StreamReader.cpp \
	@DVISVGM_TREE@/src/StreamWriter.cpp \
	@DVISVGM_TREE@/src/Subfont.cpp \
	@DVISVGM_TREE@/src/System.cpp \
	@DVISVGM_TREE@/src/TFM.cpp \
	@DVISVGM_TREE@/src/TensorProductPatch.cpp \
	@DVISVGM_TREE@/src/Terminal.cpp \
	@DVISVGM_TREE@/src/ToUnicodeMap.cpp \
	@DVISVGM_TREE@/src/TpicSpecialHandler.cpp \
	@DVISVGM_TREE@/src/TriangularPatch.cpp \
	@DVISVGM_TREE@/src/Unicode.cpp \
	@DVISVGM_TREE@/src/VFReader.cpp \
	@DVISVGM_TREE@/src/XMLDocument.cpp \
	@DVISVGM_TREE@/src/XMLNode.cpp \
	@DVISVGM_TREE@/src/XMLString.cpp

if dlloader
nodist_libdvisvgm_a_SOURCES += \
	@DVISVGM_TREE@/src/DLLoader.cpp
endif dlloader

$(libdvisvgm_a_OBJECTS): $(POTRACE_DEPEND)

nodist_libclipper_a_SOURCES = \
	@DVISVGM_TREE@/clipper/clipper.cpp

config.force: $(dvisvgm_dependencies)
	echo timestamp >config.force
	$(SHELL) ./config.status --recheck

DISTCLEANFILES = config.force CXXLD.sh

## Rebuild libkpathsea
@KPATHSEA_RULE@
## Rebuild libpotrace
@POTRACE_RULE@
## Rebuild libfreetype
@FREETYPE2_RULE@
## Rebuild libz
@ZLIB_RULE@

## Tests.
##
TESTS = tests/dvisvgm.test
EXTRA_DIST += $(TESTS)

