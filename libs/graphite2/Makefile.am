## Proxy Makefile.am to build graphite2 for TeX Live.
##
##   Copyright (C) 2012-2015 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
## We want to re-distribute the whole original graphite2 source tree.
##
EXTRA_DIST = $(GRAPHITE2_TREE)

## Changes applied to the original source tree
##
EXTRA_DIST += $(GRAPHITE2_TREE)-PATCHES

GRAPHITE2_SRC = $(GRAPHITE2_TREE)/src
GRAPHITE2_INC = $(GRAPHITE2_TREE)/include

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

SUBDIRS = . include/graphite2

noinst_LIBRARIES = libgraphite2.a

## common
AM_CPPFLAGS = -I$(top_srcdir)/$(GRAPHITE2_SRC) -I$(top_srcdir)/$(GRAPHITE2_INC)
AM_CPPFLAGS += -DGRAPHITE2_STATIC -DGRAPHITE2_CUSTOM_HEADER='<config.h>'
libgraphite2_a_SOURCES =  \
	@GRAPHITE2_TREE@/src/Bidi.cpp \
	@GRAPHITE2_TREE@/src/CachedFace.cpp \
	@GRAPHITE2_TREE@/src/CmapCache.cpp \
	@GRAPHITE2_TREE@/src/Code.cpp \
	@GRAPHITE2_TREE@/src/Face.cpp \
	@GRAPHITE2_TREE@/src/FeatureMap.cpp \
	@GRAPHITE2_TREE@/src/Font.cpp \
	@GRAPHITE2_TREE@/src/GlyphCache.cpp \
	@GRAPHITE2_TREE@/src/GlyphFace.cpp \
	@GRAPHITE2_TREE@/src/Justifier.cpp \
	@GRAPHITE2_TREE@/src/NameTable.cpp \
	@GRAPHITE2_TREE@/src/Pass.cpp \
	@GRAPHITE2_TREE@/src/Rule.cpp \
	@GRAPHITE2_TREE@/src/Segment.cpp \
	@GRAPHITE2_TREE@/src/Silf.cpp \
	@GRAPHITE2_TREE@/src/Slot.cpp \
	@GRAPHITE2_TREE@/src/Sparse.cpp \
	@GRAPHITE2_TREE@/src/TtfUtil.cpp \
	@GRAPHITE2_TREE@/src/UtfCodec.cpp \
	@GRAPHITE2_TREE@/src/gr_char_info.cpp \
	@GRAPHITE2_TREE@/src/gr_face.cpp \
	@GRAPHITE2_TREE@/src/gr_features.cpp \
	@GRAPHITE2_TREE@/src/gr_font.cpp \
	@GRAPHITE2_TREE@/src/gr_logging.cpp \
	@GRAPHITE2_TREE@/src/gr_segment.cpp \
	@GRAPHITE2_TREE@/src/gr_slot.cpp \
	@GRAPHITE2_TREE@/src/inc/CachedFace.h \
	@GRAPHITE2_TREE@/src/inc/CharInfo.h \
	@GRAPHITE2_TREE@/src/inc/CmapCache.h \
	@GRAPHITE2_TREE@/src/inc/Code.h \
	@GRAPHITE2_TREE@/src/inc/Endian.h \
	@GRAPHITE2_TREE@/src/inc/Face.h \
	@GRAPHITE2_TREE@/src/inc/FeatureMap.h \
	@GRAPHITE2_TREE@/src/inc/FeatureVal.h \
	@GRAPHITE2_TREE@/src/inc/FileFace.h \
	@GRAPHITE2_TREE@/src/inc/Font.h \
	@GRAPHITE2_TREE@/src/inc/GlyphCache.h \
	@GRAPHITE2_TREE@/src/inc/GlyphFace.h \
	@GRAPHITE2_TREE@/src/inc/List.h \
	@GRAPHITE2_TREE@/src/inc/Machine.h \
	@GRAPHITE2_TREE@/src/inc/Main.h \
	@GRAPHITE2_TREE@/src/inc/NameTable.h \
	@GRAPHITE2_TREE@/src/inc/Pass.h \
	@GRAPHITE2_TREE@/src/inc/Position.h \
	@GRAPHITE2_TREE@/src/inc/Rule.h \
	@GRAPHITE2_TREE@/src/inc/SegCache.h \
	@GRAPHITE2_TREE@/src/inc/SegCacheEntry.h \
	@GRAPHITE2_TREE@/src/inc/SegCacheStore.h \
	@GRAPHITE2_TREE@/src/inc/Segment.h \
	@GRAPHITE2_TREE@/src/inc/Silf.h \
	@GRAPHITE2_TREE@/src/inc/Slot.h \
	@GRAPHITE2_TREE@/src/inc/Sparse.h \
	@GRAPHITE2_TREE@/src/inc/TtfTypes.h \
	@GRAPHITE2_TREE@/src/inc/TtfUtil.h \
	@GRAPHITE2_TREE@/src/inc/UtfCodec.h \
	@GRAPHITE2_TREE@/src/inc/bits.h \
	@GRAPHITE2_TREE@/src/inc/debug.h \
	@GRAPHITE2_TREE@/src/inc/json.h \
	@GRAPHITE2_TREE@/src/inc/locale2lcid.h \
	@GRAPHITE2_TREE@/src/inc/opcode_table.h \
	@GRAPHITE2_TREE@/src/inc/opcodes.h

if DIRECT_TYPE
libgraphite2_a_SOURCES += @GRAPHITE2_TREE@/src/direct_machine.cpp
else !DIRECT_TYPE
libgraphite2_a_SOURCES += @GRAPHITE2_TREE@/src/call_machine.cpp
endif !DIRECT_TYPE

if WITH_SEGCACHE
libgraphite2_a_SOURCES += \
	@GRAPHITE2_TREE@/src/SegCache.cpp \
	@GRAPHITE2_TREE@/src/SegCacheEntry.cpp \
	@GRAPHITE2_TREE@/src/SegCacheStore.cpp
else !WITH_SEGCACHE
AM_CPPFLAGS += -DGRAPHITE2_NSEGCACHE
endif !WITH_SEGCACHE

if WITH_FILEFACE
libgraphite2_a_SOURCES += @GRAPHITE2_TREE@/src/FileFace.cpp 
else !WITH_FILEFACE
AM_CPPFLAGS += -DGRAPHITE2_NFILEFACE
endif !WITH_FILEFACE

if WITH_TRACING
libgraphite2_a_SOURCES += @GRAPHITE2_TREE@/src/json.cpp
else !WITH_TRACING
AM_CPPFLAGS += -DGRAPHITE2_NTRACING
endif !WITH_TRACING

if build
check_PROGRAMS = gr2test
dist_check_SCRIPTS = graphite2.test
TESTS = graphite2.test
endif build

gr2test_SOURCES = gr2test.c

# Force Automake to use CXXLD for linking
nodist_EXTRA_gr2test_SOURCES = dummy.cxx

gr2test_CPPFLAGS = -Iinclude -DGRAPHITE2_STATIC

LDADD = libgraphite2.a

# Rebuild
rebuild_prereq =
rebuild_target = all
CLEANFILES =

include $(srcdir)/../../am/rebuild.am

