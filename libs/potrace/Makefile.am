## $Id$
## Proxy Makefile.am to build potrace for TeX Live.
##
##   Copyright 2016-2017 Karl Berry <tex-live@tug.org>
##   Copyright 2013-2015 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
## We want to re-distribute the whole original potrace source tree.
##
EXTRA_DIST = $(POTRACE_TREE)

## Changes applied to the original source tree
##
EXTRA_DIST += TLpatches

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

SUBDIRS = . include

AM_CPPFLAGS = -I$(top_srcdir)/$(POTRACE_TREE)/src

noinst_LIBRARIES = libpotrace.a

libpotrace_a_SOURCES = \
	@POTRACE_TREE@/src/auxiliary.h \
	@POTRACE_TREE@/src/bitmap.h \
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

