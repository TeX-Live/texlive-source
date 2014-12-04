## Proxy Makefile.am to build zziplib for TeX Live.
##
##   Copyright (C) 2010-2014 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
## We want to re-distribute the whole zziplib source tree.
##
EXTRA_DIST = $(ZZIPLIB_TREE)

## Changes applied to the original source tree
##
EXTRA_DIST += $(ZZIPLIB_TREE)-PATCHES

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn -o -name .deps -o -name .dirstamp -o -name '*.$(OBJEXT)'` \
	  $(distdir)/$(ZZIPLIB_TREE)/uses
	rm -f $(distdir)/$(ZZIPLIB_TREE)/aclocal.m4 \
	  $(distdir)/$(ZZIPLIB_TREE)/zzip/_config.h \
	  $(distdir)/$(ZZIPLIB_TREE)/zzip/_msvc.h

SUBDIRS = . include/zzip

AM_CPPFLAGS = -Iinclude -I$(top_srcdir)/$(ZZIPLIB_TREE) $(ZLIB_INCLUDES)
AM_CPPFLAGS += $(ZZIPLIB_DEFINES)
AM_CFLAGS = $(VISIBILITY_CFLAGS) $(WARNING_CFLAGS)

noinst_LIBRARIES=libzzip.a

nodist_libzzip_a_SOURCES = \
	@ZZIPLIB_TREE@/zzip/dir.c \
	@ZZIPLIB_TREE@/zzip/err.c \
	@ZZIPLIB_TREE@/zzip/fetch.c \
	@ZZIPLIB_TREE@/zzip/file.c \
	@ZZIPLIB_TREE@/zzip/info.c \
	@ZZIPLIB_TREE@/zzip/plugin.c \
	@ZZIPLIB_TREE@/zzip/stat.c \
	@ZZIPLIB_TREE@/zzip/write.c \
	@ZZIPLIB_TREE@/zzip/zip.c

$(libzzip_a_OBJECTS): config.force

DISTCLEANFILES = include/_msvc.in _configs.sed

TESTS_ENVIRONMENT = ZZIPLIB_TREE=$(ZZIPLIB_TREE)
if build
TESTS = zziplib.test
check_PROGRAMS = unzzip
dist_check_SCRIPTS = zziplib.test
endif build

distclean-local:
	rm -rf test

nodist_unzzip_SOURCES = @ZZIPLIB_TREE@/bins/unzzip.c
unzzip_LDADD = libzzip.a $(ZLIB_LIBS)

## Rebuild zlib
@ZLIB_RULE@

# Reconfig
reconfig_prereq = $(ZLIB_DEPEND)

include $(srcdir)/../../am/reconfig.am

# Rebuild
rebuild_prereq =
rebuild_target = all
CLEANFILES =

include $(srcdir)/../../am/rebuild.am

