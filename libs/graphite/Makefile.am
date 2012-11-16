## Makefile.am for the TeX Live subdirectory libs/graphite/
##
## Copyright (C) 2009-2012 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

# Rebuild
.PHONY: rebuild
rebuild: all

## We want to re-distribute the whole original silgraphite/engine source tree.
##
## With current automake (1.10.2) 'make distcheck' fails when
## DISTFILES contains a directory and files in that directory.
## Thus nodist_* for all files in $(TECKIT_TREE).
EXTRA_DIST = $(GRAPHITE_TREE)

## Patches applied to the original source tree
##
EXTRA_DIST += $(GRAPHITE_TREE)-PATCHES

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`

SUBDIRS = include/graphite

AM_CPPFLAGS = -I$(top_srcdir)/$(GRAPHITE_TREE)/include/graphite -I$(top_srcdir)/$(GRAPHITE_TREE)/src/generic
AM_CPPFLAGS += -I$(top_srcdir)/$(GRAPHITE_TREE)/src/font -I$(top_srcdir)/$(GRAPHITE_TREE)/src/segment
AM_CPPFLAGS += -DNDEBUG
AM_CFLAGS = $(WARNING_CFLAGS)
AM_CXXFLAGS = $(WARNING_CXXFLAGS)

noinst_LIBRARIES = libgraphite.a

nodist_libgraphite_a_SOURCES = \
	$(GRAPHITE_TREE)/src/font/FileFont.cpp \
	$(GRAPHITE_TREE)/src/font/Font.cpp \
	$(GRAPHITE_TREE)/src/font/TtfUtil.cpp \
	$(GRAPHITE_TREE)/src/generic/Debug.cpp \
	$(GRAPHITE_TREE)/src/generic/Util.cpp \
	$(GRAPHITE_TREE)/src/painter/SegmentPainter.cpp \
	$(GRAPHITE_TREE)/src/segment/FileInput.cpp \
	$(GRAPHITE_TREE)/src/segment/FontCache.cpp \
	$(GRAPHITE_TREE)/src/segment/FontFace.cpp \
	$(GRAPHITE_TREE)/src/segment/GrCharStream.cpp \
	$(GRAPHITE_TREE)/src/segment/GrClassTable.cpp \
	$(GRAPHITE_TREE)/src/segment/GrEngine.cpp \
	$(GRAPHITE_TREE)/src/segment/GrFSM.cpp \
	$(GRAPHITE_TREE)/src/segment/GrFeature.cpp \
	$(GRAPHITE_TREE)/src/segment/GrGlyphTable.cpp \
	$(GRAPHITE_TREE)/src/segment/GrPass.cpp \
	$(GRAPHITE_TREE)/src/segment/GrPassActionCode.cpp \
	$(GRAPHITE_TREE)/src/segment/GrSlotState.cpp \
	$(GRAPHITE_TREE)/src/segment/GrSlotStream.cpp \
	$(GRAPHITE_TREE)/src/segment/GrTableManager.cpp \
	$(GRAPHITE_TREE)/src/segment/MemoryUsage.cpp \
	$(GRAPHITE_TREE)/src/segment/Platform.cpp \
	$(GRAPHITE_TREE)/src/segment/Segment.cpp \
	$(GRAPHITE_TREE)/src/segment/SegmentAux.cpp \
	$(GRAPHITE_TREE)/src/segment/TransductionLog.cpp

