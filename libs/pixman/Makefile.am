## Proxy Makefile.am to build pixman for TeX Live.
##
##   Copyright (C) 2012 Taco Hoekwater <taco@metatex.org>
##   Copyright (C) 2012-2014 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
## We want to re-distribute the whole pixman source tree.
##
EXTRA_DIST = $(PIXMAN_TREE)

## Changes applied to the original source tree
##
EXTRA_DIST += $(PIXMAN_TREE)-PATCHES

PIXMAN_SRC = $(PIXMAN_TREE)/pixman

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn -o -name .deps -o -name .dirstamp -o -name '*.$(OBJEXT)'`
	rm -f $(distdir)/$(PIXMAN_SRC)/pixman-version.h

SUBDIRS = . include

AM_CPPFLAGS = -I$(top_srcdir)/$(PIXMAN_SRC)
AM_CFLAGS = $(VISIBILITY_CFLAGS) # $(WARNING_CFLAGS)

noinst_LIBRARIES = libpixman.a

include $(srcdir)/sources.am

libpixman_a_SOURCES = $(libpixman_sources) $(libpixman_headers)

if build
check_PROGRAMS = pixtest
dist_check_SCRIPTS = pixman.test
TESTS = pixman.test
endif build

pixtest_SOURCES = pixtest.c

pixtest_CPPFLAGS = -Iinclude

LDADD = libpixman.a

# Rebuild
rebuild_prereq =
rebuild_target = all
CLEANFILES =

include $(srcdir)/../../am/rebuild.am

