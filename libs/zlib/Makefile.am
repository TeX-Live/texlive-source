## Proxy Makefile.am to build zlib for TeX Live.
##
##   Copyright (C) 2009-2015 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
## We want to re-distribute the whole zlib source tree.
##
EXTRA_DIST = $(ZLIB_TREE)

## Changes applied to the original source tree
##
EXTRA_DIST += $(ZLIB_TREE)-PATCHES

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

SUBDIRS = . include

AM_CPPFLAGS = -I$(top_srcdir)/$(ZLIB_TREE) $(ZLIB_DEFINES)
AM_CFLAGS = $(VISIBILITY_CFLAGS) # $(WARNING_CFLAGS)

noinst_LIBRARIES = libz.a

nodist_libz_a_SOURCES = \
	@ZLIB_TREE@/adler32.c \
	@ZLIB_TREE@/compress.c \
	@ZLIB_TREE@/crc32.c \
	@ZLIB_TREE@/crc32.h \
	@ZLIB_TREE@/deflate.c \
	@ZLIB_TREE@/deflate.h \
	@ZLIB_TREE@/infback.c \
	@ZLIB_TREE@/gzclose.c \
	@ZLIB_TREE@/gzguts.h \
	@ZLIB_TREE@/gzlib.c \
	@ZLIB_TREE@/gzread.c \
	@ZLIB_TREE@/gzwrite.c \
	@ZLIB_TREE@/inffast.c \
	@ZLIB_TREE@/inffast.h \
	@ZLIB_TREE@/inffixed.h \
	@ZLIB_TREE@/inflate.c \
	@ZLIB_TREE@/inflate.h \
	@ZLIB_TREE@/inftrees.c \
	@ZLIB_TREE@/inftrees.h \
	@ZLIB_TREE@/trees.c \
	@ZLIB_TREE@/trees.h \
	@ZLIB_TREE@/uncompr.c \
	@ZLIB_TREE@/zutil.h \
	@ZLIB_TREE@/zutil.c

$(libz_a_OBJECTS): zconf.h

## Tests
##
if build
check_PROGRAMS = example minigzip
dist_check_SCRIPTS = zlib.test
TESTS = zlib.test
endif build
CLEANFILES = foo.gz foo_gz

LDADD = libz.a

nodist_example_SOURCES = @ZLIB_TREE@/test/example.c
nodist_minigzip_SOURCES = @ZLIB_TREE@/test/minigzip.c

zconf.h: $(ZLIB_TREE)/zconf.h.in config.status
	echo '$(SHELL) ./config.status $@'
	$(SHELL) ./config.status $@

DISTCLEANFILES = zconf.h

# Rebuild
rebuild_prereq =
rebuild_target = all

include $(srcdir)/../../am/rebuild.am

