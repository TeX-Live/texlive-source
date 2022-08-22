## $Id: Makefile.am 57588 2021-02-01 06:11:18Z kakuto $
## Proxy Makefile.am to build potrace for TeX Live.
##
##   Copyright 2022 Luigi Scarso <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##

ACLOCAL_AMFLAGS = -I ../../m4

## We want to re-distribute the whole original potrace source tree.
##
EXTRA_DIST = $(POTRACE_TREE)


## Actual patches applied to the original source tree
EXTRA_DIST += TLpatches

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

SUBDIRS = . include

AM_CPPFLAGS = -I$(top_srcdir)/$(POTRACE_TREE)/src
AM_CFLAGS = $(WARNING_CFLAGS)
AM_CXXFLAGS = $(NO_WARN_CXXFLAGS)

noinst_LIBRARIES = libpotrace.a

nodist_libpotrace_a_SOURCES = \
	@POTRACE_TREE@/src/auxiliary.h \
	@POTRACE_TREE@/src/bitmap.h \
	@POTRACE_TREE@/src/config.h \
	@POTRACE_TREE@/src/curve.c \
	@POTRACE_TREE@/src/curve.h \
	@POTRACE_TREE@/src/decompose.c \
	@POTRACE_TREE@/src/decompose.h \
	@POTRACE_TREE@/src/lists.h \
	@POTRACE_TREE@/src/potracelib.c \
	@POTRACE_TREE@/src/potracelib.h \
	@POTRACE_TREE@/src/progress.h \
	@POTRACE_TREE@/src/trace.c \
	@POTRACE_TREE@/src/trace.h 


unused_sources = \
	@POTRACE_TREE@/src/backend_dxf.c \
	@POTRACE_TREE@/src/backend_dxf.h \
	@POTRACE_TREE@/src/backend_eps.c \
	@POTRACE_TREE@/src/backend_eps.h \
	@POTRACE_TREE@/src/backend_geojson.c \
	@POTRACE_TREE@/src/backend_geojson.h \
	@POTRACE_TREE@/src/backend_pdf.c \
	@POTRACE_TREE@/src/backend_pdf.h \
	@POTRACE_TREE@/src/backend_pgm.c \
	@POTRACE_TREE@/src/backend_pgm.h \
	@POTRACE_TREE@/src/backend_svg.c \
	@POTRACE_TREE@/src/backend_svg.h \
	@POTRACE_TREE@/src/backend_xfig.c \
	@POTRACE_TREE@/src/backend_xfig.h \
	@POTRACE_TREE@/src/bbox.c \
	@POTRACE_TREE@/src/bbox.h \
	@POTRACE_TREE@/src/bitmap_io.c \
	@POTRACE_TREE@/src/bitmap_io.h \
	@POTRACE_TREE@/src/bitops.h \
	@POTRACE_TREE@/src/flate.c \
	@POTRACE_TREE@/src/flate.h \
	@POTRACE_TREE@/src/getopt.c \
	@POTRACE_TREE@/src/getopt1.c \
	@POTRACE_TREE@/src/greymap.c \
	@POTRACE_TREE@/src/greymap.h \
	@POTRACE_TREE@/src/include/getopt/getopt.h \
	@POTRACE_TREE@/src/lzw.c \
	@POTRACE_TREE@/src/lzw.h \
	@POTRACE_TREE@/src/main.c \
	@POTRACE_TREE@/src/main.h \
	@POTRACE_TREE@/src/mkbitmap.c \
	@POTRACE_TREE@/src/platform.h \
	@POTRACE_TREE@/src/potracelib_demo.c \
	@POTRACE_TREE@/src/progress_bar.c \
	@POTRACE_TREE@/src/progress_bar.h \
	@POTRACE_TREE@/src/render.c \
	@POTRACE_TREE@/src/render.h \
	@POTRACE_TREE@/src/trans.c \
	@POTRACE_TREE@/src/trans.h 


if build
check_PROGRAMS = potracetst
dist_check_SCRIPTS = potrace.test
TESTS = potrace.test
endif build
potrace.log: potracetst$(EXEEXT)

potracetst_SOURCES = potracetst.c

potracetst_CPPFLAGS = -Iinclude

LDADD = libpotrace.a


# Rebuild
rebuild_prereq =
rebuild_target = all
CLEANFILES =

include $(srcdir)/../../am/rebuild.am
