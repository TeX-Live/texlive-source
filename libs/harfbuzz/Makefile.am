## Proxy Makefile.am to build harfbuzz for TeX Live.
##
##   Copyright (C) 2012-2015 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
## We want to re-distribute the whole original harfbuzz source tree.
##
EXTRA_DIST = $(HARFBUZZ_TREE)

## Changes applied to the original source tree
##
EXTRA_DIST += $(HARFBUZZ_TREE)-PATCHES

HARFBUZZ_SRC = $(HARFBUZZ_TREE)/src

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

SUBDIRS = . include

noinst_LIBRARIES = libharfbuzz.a

## common
AM_CPPFLAGS = -DHB_NO_MT -DHB_NO_UNICODE_FUNCS -I$(top_srcdir)/$(HARFBUZZ_SRC)
libharfbuzz_dependencies =
libharfbuzz_a_SOURCES =  \
	@HARFBUZZ_TREE@/src/hb-atomic-private.hh \
	@HARFBUZZ_TREE@/src/hb-blob.cc \
	@HARFBUZZ_TREE@/src/hb-buffer-deserialize-json.hh \
	@HARFBUZZ_TREE@/src/hb-buffer-deserialize-text.hh \
	@HARFBUZZ_TREE@/src/hb-buffer-private.hh \
	@HARFBUZZ_TREE@/src/hb-buffer-serialize.cc \
	@HARFBUZZ_TREE@/src/hb-buffer.cc \
	@HARFBUZZ_TREE@/src/hb-cache-private.hh \
	@HARFBUZZ_TREE@/src/hb-common.cc \
	@HARFBUZZ_TREE@/src/hb-face-private.hh \
	@HARFBUZZ_TREE@/src/hb-face.cc \
	@HARFBUZZ_TREE@/src/hb-fallback-shape.cc \
	@HARFBUZZ_TREE@/src/hb-font-private.hh \
	@HARFBUZZ_TREE@/src/hb-font.cc \
	@HARFBUZZ_TREE@/src/hb-mutex-private.hh \
	@HARFBUZZ_TREE@/src/hb-object-private.hh \
	@HARFBUZZ_TREE@/src/hb-open-file-private.hh \
	@HARFBUZZ_TREE@/src/hb-open-type-private.hh \
	@HARFBUZZ_TREE@/src/hb-ot-head-table.hh \
	@HARFBUZZ_TREE@/src/hb-ot-hhea-table.hh \
	@HARFBUZZ_TREE@/src/hb-ot-hmtx-table.hh \
	@HARFBUZZ_TREE@/src/hb-ot-maxp-table.hh \
	@HARFBUZZ_TREE@/src/hb-ot-name-table.hh \
	@HARFBUZZ_TREE@/src/hb-ot-os2-table.hh \
	@HARFBUZZ_TREE@/src/hb-ot-tag.cc \
	@HARFBUZZ_TREE@/src/hb-private.hh \
	@HARFBUZZ_TREE@/src/hb-set-private.hh \
	@HARFBUZZ_TREE@/src/hb-set.cc \
	@HARFBUZZ_TREE@/src/hb-shape.cc \
	@HARFBUZZ_TREE@/src/hb-shape-plan-private.hh \
	@HARFBUZZ_TREE@/src/hb-shape-plan.cc \
	@HARFBUZZ_TREE@/src/hb-shaper-list.hh \
	@HARFBUZZ_TREE@/src/hb-shaper-impl-private.hh \
	@HARFBUZZ_TREE@/src/hb-shaper-private.hh \
	@HARFBUZZ_TREE@/src/hb-shaper.cc \
	@HARFBUZZ_TREE@/src/hb-unicode-private.hh \
	@HARFBUZZ_TREE@/src/hb-unicode.cc \
	@HARFBUZZ_TREE@/src/hb-utf-private.hh \
	@HARFBUZZ_TREE@/src/hb-warning.cc

## native OpenType Layout backend
libharfbuzz_a_SOURCES += \
	@HARFBUZZ_TREE@/src/hb-ot-font.cc \
	@HARFBUZZ_TREE@/src/hb-ot-font.h \
	@HARFBUZZ_TREE@/src/hb-ot-layout.cc \
	@HARFBUZZ_TREE@/src/hb-ot-layout-common-private.hh \
	@HARFBUZZ_TREE@/src/hb-ot-layout-gdef-table.hh \
	@HARFBUZZ_TREE@/src/hb-ot-layout-gpos-table.hh \
	@HARFBUZZ_TREE@/src/hb-ot-layout-gsubgpos-private.hh \
	@HARFBUZZ_TREE@/src/hb-ot-layout-gsub-table.hh \
	@HARFBUZZ_TREE@/src/hb-ot-layout-jstf-table.hh \
	@HARFBUZZ_TREE@/src/hb-ot-layout-private.hh \
	@HARFBUZZ_TREE@/src/hb-ot-map.cc \
	@HARFBUZZ_TREE@/src/hb-ot-map-private.hh \
	@HARFBUZZ_TREE@/src/hb-ot-shape.cc \
	@HARFBUZZ_TREE@/src/hb-ot-shape-complex-arabic.cc \
	@HARFBUZZ_TREE@/src/hb-ot-shape-complex-arabic-fallback.hh \
	@HARFBUZZ_TREE@/src/hb-ot-shape-complex-arabic-table.hh \
	@HARFBUZZ_TREE@/src/hb-ot-shape-complex-arabic-private.hh \
	@HARFBUZZ_TREE@/src/hb-ot-shape-complex-default.cc \
	@HARFBUZZ_TREE@/src/hb-ot-shape-complex-hangul.cc \
	@HARFBUZZ_TREE@/src/hb-ot-shape-complex-hebrew.cc \
	@HARFBUZZ_TREE@/src/hb-ot-shape-complex-indic.cc \
	@HARFBUZZ_TREE@/src/hb-ot-shape-complex-indic-machine.hh \
	@HARFBUZZ_TREE@/src/hb-ot-shape-complex-indic-private.hh \
	@HARFBUZZ_TREE@/src/hb-ot-shape-complex-indic-table.cc \
	@HARFBUZZ_TREE@/src/hb-ot-shape-complex-myanmar.cc \
	@HARFBUZZ_TREE@/src/hb-ot-shape-complex-myanmar-machine.hh \
	@HARFBUZZ_TREE@/src/hb-ot-shape-complex-private.hh \
	@HARFBUZZ_TREE@/src/hb-ot-shape-complex-thai.cc \
	@HARFBUZZ_TREE@/src/hb-ot-shape-complex-tibetan.cc \
	@HARFBUZZ_TREE@/src/hb-ot-shape-complex-use.cc \
	@HARFBUZZ_TREE@/src/hb-ot-shape-complex-use-machine.hh \
	@HARFBUZZ_TREE@/src/hb-ot-shape-complex-use-private.hh \
	@HARFBUZZ_TREE@/src/hb-ot-shape-complex-use-table.cc \
	@HARFBUZZ_TREE@/src/hb-ot-shape-fallback.cc \
	@HARFBUZZ_TREE@/src/hb-ot-shape-fallback-private.hh \
	@HARFBUZZ_TREE@/src/hb-ot-shape-normalize.cc \
	@HARFBUZZ_TREE@/src/hb-ot-shape-normalize-private.hh \
	@HARFBUZZ_TREE@/src/hb-ot-shape-private.hh

## ICU library
AM_CPPFLAGS += $(ICU_INCLUDES)
libharfbuzz_dependencies += $(ICU_DEPEND)
libharfbuzz_a_SOURCES += @HARFBUZZ_TREE@/src/hb-icu.cc

## Graphite library
AM_CPPFLAGS += $(GRAPHITE2_INCLUDES)
libharfbuzz_dependencies += $(GRAPHITE2_DEPEND)
libharfbuzz_a_SOURCES += @HARFBUZZ_TREE@/src/hb-graphite2.cc

$(libharfbuzz_a_OBJECTS): config.force

if build
check_PROGRAMS = hbtest
dist_check_SCRIPTS = harfbuzz.test
TESTS = harfbuzz.test
endif build
harfbuzz.log: hbtest$(EXEEXT)

hbtest_SOURCES = hbtest.c

# Force Automake to use CXXLD for linking
nodist_EXTRA_hbtest_SOURCES = dummy.cxx

hbtest_CPPFLAGS = -Iinclude $(ICU_INCLUDES) $(GRAPHITE2_INCLUDES)

LDADD = libharfbuzz.a $(ICU_LIBS) $(GRAPHITE2_LIBS)

## Rebuild libsicuxxx
@ICU_RULE@
## Rebuild libgraphite2
@GRAPHITE2_RULE@

# Reconfig
reconfig_prereq = $(libharfbuzz_dependencies)
DISTCLEANFILES =

include $(srcdir)/../../am/reconfig.am

# Rebuild
rebuild_prereq =
rebuild_target = all
CLEANFILES =

include $(srcdir)/../../am/rebuild.am

