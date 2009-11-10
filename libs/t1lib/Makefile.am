## Proxy Makefile.am to build t1lib for TeX Live.
##
##   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
ACLOCAL_AMFLAGS = -I ../../m4

# Rebuild
.PHONY: rebuild
rebuild: all

## We want to re-distribute the whole original t1lib source tree.
##
## With current automake (1.10.2) 'make distcheck' fails when
## DISTFILES contains a directory and files in that directory.
## Thus nodist_* for all files in $(T1LIB_TREE).
EXTRA_DIST = $(T1LIB_TREE)

## Changes applied to the original source tree
##
EXTRA_DIST +=  $(T1LIB_TREE)-PATCHES

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`
	rm -f $(distdir)/$(T1LIB_TREE)/ac-tools/config.guess \
	  $(distdir)/$(T1LIB_TREE)/ac-tools/config.sub \
	  $(distdir)/$(T1LIB_TREE)/ac-tools/install-sh \
	  $(distdir)/$(T1LIB_TREE)/ac-tools/ltmain.sh \
	  $(distdir)/$(T1LIB_TREE)/ac-tools/mkinstalldirs \
	  $(distdir)/$(T1LIB_TREE)/lib/t1lib/t1lib.h

INCLUDES = -Ilib -I$(top_srcdir)/$(T1LIB_TREE)
AM_CPPFLAGS = $(T1LIB_DEFINES)
AM_CPPFLAGS += -DT1LIB_IDENT=\"@T1LIB_VERSIONSTRING@\"
AM_CPPFLAGS += -DGLOBAL_CONFIG_DIR=\"@T1LIB_DATA_DIR@\"

AM_CFLAGS = $(T1LIB_CFLAGS)

LDADD = libt1.a

noinst_LIBRARIES = libt1.a

nodist_libt1_a_SOURCES = $(lib_t1lib_sources) $(lib_type1_sources)

lib_t1lib_sources = \
	$(T1LIB_TREE)/lib/t1lib/parseAFM.c \
	$(T1LIB_TREE)/lib/t1lib/t1aaset.c \
	$(T1LIB_TREE)/lib/t1lib/t1afmtool.c \
	$(T1LIB_TREE)/lib/t1lib/t1base.c \
	$(T1LIB_TREE)/lib/t1lib/t1delete.c \
	$(T1LIB_TREE)/lib/t1lib/t1enc.c \
	$(T1LIB_TREE)/lib/t1lib/t1env.c \
	$(T1LIB_TREE)/lib/t1lib/t1finfo.c \
	$(T1LIB_TREE)/lib/t1lib/t1load.c \
	$(T1LIB_TREE)/lib/t1lib/t1outline.c \
	$(T1LIB_TREE)/lib/t1lib/t1set.c \
	$(T1LIB_TREE)/lib/t1lib/t1subset.c \
	$(T1LIB_TREE)/lib/t1lib/t1trans.c

lib_type1_sources = \
	$(T1LIB_TREE)/lib/type1/arith.c \
	$(T1LIB_TREE)/lib/type1/bstring.c \
	$(T1LIB_TREE)/lib/type1/curves.c \
	$(T1LIB_TREE)/lib/type1/fontfcn.c \
	$(T1LIB_TREE)/lib/type1/hints.c \
	$(T1LIB_TREE)/lib/type1/lines.c \
	$(T1LIB_TREE)/lib/type1/objects.c \
	$(T1LIB_TREE)/lib/type1/paths.c \
	$(T1LIB_TREE)/lib/type1/regions.c \
	$(T1LIB_TREE)/lib/type1/scanfont.c \
	$(T1LIB_TREE)/lib/type1/spaces.c \
	$(T1LIB_TREE)/lib/type1/t1io.c \
	$(T1LIB_TREE)/lib/type1/t1snap.c \
	$(T1LIB_TREE)/lib/type1/t1stub.c \
	$(T1LIB_TREE)/lib/type1/token.c \
	$(T1LIB_TREE)/lib/type1/type1.c \
	$(T1LIB_TREE)/lib/type1/util.c

if build
check_PROGRAMS = type1afm
dist_check_SCRIPTS = type1afm.test
TESTS = type1afm.test
endif build

nodist_type1afm_SOURCES = $(T1LIB_TREE)/type1afm/type1afm.c

EXTRA_DIST += l047016t.pfa l047016t.xafm

CLEANFILES = l047016t.afmx x047016t.afm x047016t.pfa

