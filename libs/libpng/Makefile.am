## Proxy Makefile.am to build libpng for TeX Live.
##
##   Copyright (C) 2009-2015 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
## We want to re-distribute the whole libpng source tree.
##
EXTRA_DIST = $(LIBPNG_TREE)

## Changes applied to the original source tree
##
EXTRA_DIST += $(LIBPNG_TREE)-PATCHES

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

SUBDIRS = . include

AM_CPPFLAGS = -I$(top_srcdir)/$(LIBPNG_TREE) $(ZLIB_INCLUDES) $(LIBPNG_DEFINES)
AM_CFLAGS = $(VISIBILITY_CFLAGS) $(WARNING_CFLAGS)

noinst_LIBRARIES=libpng.a

nodist_libpng_a_SOURCES = \
	@LIBPNG_TREE@/png.c \
	@LIBPNG_TREE@/pngset.c \
	@LIBPNG_TREE@/pngget.c \
	@LIBPNG_TREE@/pngrutil.c \
	@LIBPNG_TREE@/pngtrans.c \
	@LIBPNG_TREE@/pngwutil.c \
	@LIBPNG_TREE@/pngread.c \
	@LIBPNG_TREE@/pngrio.c \
	@LIBPNG_TREE@/pngwio.c \
	@LIBPNG_TREE@/pngwrite.c \
	@LIBPNG_TREE@/pngrtran.c \
	@LIBPNG_TREE@/pngwtran.c \
	@LIBPNG_TREE@/pngmem.c \
	@LIBPNG_TREE@/pngerror.c \
	@LIBPNG_TREE@/pngpread.c

$(libpng_a_OBJECTS): config.force

## Tests
##
TESTS_ENVIRONMENT = LIBPNG_TREE=$(LIBPNG_TREE)
if build
TESTS = libpng.test
check_PROGRAMS = pngtest
dist_check_SCRIPTS = libpng.test
endif build
libpng.log: pngtest$(EXEEXT)

CLEANFILES = pngout.png

nodist_pngtest_SOURCES = @LIBPNG_TREE@/pngtest.c
pngtest_LDADD = libpng.a $(ZLIB_LIBS)

## Rebuild zlib
@ZLIB_RULE@

# Reconfig
reconfig_prereq = $(ZLIB_DEPEND)
DISTCLEANFILES =

include $(srcdir)/../../am/reconfig.am

# Rebuild
rebuild_prereq =
rebuild_target = all

include $(srcdir)/../../am/rebuild.am

