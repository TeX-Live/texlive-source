## $Id$
## Proxy Makefile.am to build harfbuzz for TeX Live.
##
##   Copyright 2016-2017 Karl Berry <tex-live@tug.org>
##   Copyright 2012-2015 Peter Breitenlohner <tex-live@tug.org>
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
EXTRA_DIST += TLpatches

HARFBUZZ_SRC = $(HARFBUZZ_TREE)/src

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

SUBDIRS = . include

noinst_LIBRARIES = libharfbuzz.a

## common
AM_CPPFLAGS = -DHB_NO_MT -DHAVE_FALLBACK=1 \
	-I$(top_srcdir)/$(HARFBUZZ_SRC)
## For luahbtex we need this (and the rest ?)
AM_CXXFLAGS = $(VISIBILITY_CXXFLAGS) $(WARNING_CXXFLAGS)

libharfbuzz_dependencies =
libharfbuzz_a_SOURCES = @HARFBUZZ_TREE@/src/harfbuzz.cc

## Graphite library
AM_CPPFLAGS += $(GRAPHITE2_INCLUDES)
libharfbuzz_dependencies += $(GRAPHITE2_DEPEND)

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

hbtest_CPPFLAGS = -Iinclude $(GRAPHITE2_INCLUDES)

LDADD = libharfbuzz.a $(GRAPHITE2_LIBS)

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

