## Makefile.am for the TeX Live subdirectory texk/dvisvgm/
##
## Copyright (C) 2009-2012 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
#************************************************************************
#
#  Adapted for TeX Live from dvisvgm-0.8.7/Makefile.am
#  Copyright (C) 2005-2009 Martin Gieseking <martin.gieseking@uos.de>
#
#************************************************************************
##
ACLOCAL_AMFLAGS = -I ../../m4

## We want to re-distribute the whole original dvisvgm source tree.
##
EXTRA_DIST = $(DVISVGM_TREE)

## Patches applied to the original source tree
##
EXTRA_DIST += $(DVISVGM_TREE)-PATCHES

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`

AM_CPPFLAGS = -I$(top_srcdir)/$(DVISVGM_TREE)/src
AM_CPPFLAGS += -I$(top_srcdir)/$(DVISVGM_TREE)/potracelib
AM_CPPFLAGS += $(KPATHSEA_INCLUDES) $(FREETYPE2_INCLUDES)
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
	$(DVISVGM_TREE)/src/dvisvgm.cpp \
	$(DVISVGM_TREE)/src/gzstream.cpp

dvisvgm_dependencies = $(KPATHSEA_DEPEND) $(ZLIB_DEPEND) $(FREETYPE2_DEPEND)

$(dvisvgm_OBJECTS): libdvisvgm.a

LDADD = $(noinst_LIBRARIES) $(KPATHSEA_LIBS) $(FREETYPE2_LIBS)
LDADD += $(ZLIB_LIBS) $(LIBGS_LIBS)

noinst_LIBRARIES = libdvisvgm.a libpotrace.a

nodist_libdvisvgm_a_SOURCES = \
	$(DVISVGM_TREE)/src/BgColorSpecialHandler.cpp \
	$(DVISVGM_TREE)/src/Bitmap.cpp \
	$(DVISVGM_TREE)/src/BoundingBox.cpp \
	$(DVISVGM_TREE)/src/CRC32.cpp \
	$(DVISVGM_TREE)/src/Calculator.cpp \
	$(DVISVGM_TREE)/src/CmdLineParserBase.cpp \
	$(DVISVGM_TREE)/src/Color.cpp \
	$(DVISVGM_TREE)/src/ColorSpecialHandler.cpp \
	$(DVISVGM_TREE)/src/CommandLine.cpp \
	$(DVISVGM_TREE)/src/Directory.cpp \
	$(DVISVGM_TREE)/src/DVIActions.cpp \
	$(DVISVGM_TREE)/src/DVIReader.cpp \
	$(DVISVGM_TREE)/src/DvisvgmSpecialHandler.cpp \
	$(DVISVGM_TREE)/src/DVIToSVG.cpp \
	$(DVISVGM_TREE)/src/DVIToSVGActions.cpp \
	$(DVISVGM_TREE)/src/EmSpecialHandler.cpp \
	$(DVISVGM_TREE)/src/FileFinder.cpp \
	$(DVISVGM_TREE)/src/FilePath.cpp \
	$(DVISVGM_TREE)/src/FileSystem.cpp \
	$(DVISVGM_TREE)/src/Font.cpp \
	$(DVISVGM_TREE)/src/FontCache.cpp \
	$(DVISVGM_TREE)/src/FontEncoding.cpp \
	$(DVISVGM_TREE)/src/FontEngine.cpp \
	$(DVISVGM_TREE)/src/FontManager.cpp \
	$(DVISVGM_TREE)/src/FontMap.cpp \
	$(DVISVGM_TREE)/src/GFReader.cpp \
	$(DVISVGM_TREE)/src/GFGlyphTracer.cpp \
	$(DVISVGM_TREE)/src/GFTracer.cpp \
	$(DVISVGM_TREE)/src/Ghostscript.cpp \
	$(DVISVGM_TREE)/src/InputBuffer.cpp \
	$(DVISVGM_TREE)/src/InputReader.cpp \
	$(DVISVGM_TREE)/src/Length.cpp \
	$(DVISVGM_TREE)/src/Matrix.cpp \
	$(DVISVGM_TREE)/src/Message.cpp \
	$(DVISVGM_TREE)/src/MetafontWrapper.cpp \
	$(DVISVGM_TREE)/src/PageRanges.cpp \
	$(DVISVGM_TREE)/src/PageSize.cpp \
	$(DVISVGM_TREE)/src/Process.cpp \
	$(DVISVGM_TREE)/src/PSInterpreter.cpp \
	$(DVISVGM_TREE)/src/PsSpecialHandler.cpp \
	$(DVISVGM_TREE)/src/SignalHandler.cpp \
	$(DVISVGM_TREE)/src/SpecialManager.cpp \
	$(DVISVGM_TREE)/src/StreamReader.cpp \
	$(DVISVGM_TREE)/src/StreamWriter.cpp \
	$(DVISVGM_TREE)/src/SVGTree.cpp \
	$(DVISVGM_TREE)/src/System.cpp \
	$(DVISVGM_TREE)/src/Terminal.cpp \
	$(DVISVGM_TREE)/src/TFM.cpp \
	$(DVISVGM_TREE)/src/TpicSpecialHandler.cpp \
	$(DVISVGM_TREE)/src/VFReader.cpp \
	$(DVISVGM_TREE)/src/XMLDocument.cpp \
	$(DVISVGM_TREE)/src/XMLNode.cpp \
	$(DVISVGM_TREE)/src/XMLString.cpp

if dlloader
nodist_libdvisvgm_a_SOURCES += \
	$(DVISVGM_TREE)/src/DLLoader.cpp
endif dlloader

$(libdvisvgm_a_OBJECTS): libpotrace.a

nodist_libpotrace_a_SOURCES = \
	$(DVISVGM_TREE)/potracelib/curve.c \
	$(DVISVGM_TREE)/potracelib/decompose.c \
	$(DVISVGM_TREE)/potracelib/potracelib.c \
	$(DVISVGM_TREE)/potracelib/trace.c

$(libpotrace_a_OBJECTS): config.force

config.force: $(dvisvgm_dependencies)
	echo timestamp >config.force
	$(SHELL) ./config.status --recheck

DISTCLEANFILES = config.force CXXLD.sh

## Rebuild libkpathsea
@KPATHSEA_RULE@
## Rebuild libfreetype
@FREETYPE2_RULE@
## Rebuild libz
@ZLIB_RULE@

