## $Id$
## Makefile.am for the TeX Live subdirectory utils/m-tx/
##
## Copyright 2017 Karl Berry <tex-live@tug.org>
## Copyright 2012-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.

## We want to re-distribute the whole original mtx source tree.
EXTRA_DIST = $(MTX_TREE)

## Patches applied to the original source tree
EXTRA_DIST += TLpatches

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

AM_CPPFLAGS = -I$(srcdir)/$(MTX_TREE)/libp2c -DNO_DECLARE_MALLOC
AM_CFLAGS = #$(WARNING_CFLAGS)

bin_PROGRAMS = prepmx

prepmx_SOURCES = \
	@MTX_TREE@/analyze.c \
	@MTX_TREE@/analyze.h \
	@MTX_TREE@/cfuncs.c \
	@MTX_TREE@/cfuncs.h \
	@MTX_TREE@/control.c \
	@MTX_TREE@/control.h \
	@MTX_TREE@/files.c \
	@MTX_TREE@/files.h \
	@MTX_TREE@/globals.c \
	@MTX_TREE@/globals.h \
	@MTX_TREE@/libp2c/p2c.h \
	@MTX_TREE@/lyrics.c \
	@MTX_TREE@/lyrics.h \
	@MTX_TREE@/mtx.c \
	@MTX_TREE@/mtx.h \
	@MTX_TREE@/mtxline.c \
	@MTX_TREE@/mtxline.h \
	@MTX_TREE@/multfile.c \
	@MTX_TREE@/multfile.h \
	@MTX_TREE@/notes.c \
	@MTX_TREE@/notes.h \
	@MTX_TREE@/preamble.c \
	@MTX_TREE@/preamble.h \
	@MTX_TREE@/prepmx.c \
	@MTX_TREE@/status.c \
	@MTX_TREE@/status.h \
	@MTX_TREE@/strings.h \
	@MTX_TREE@/uptext.c \
	@MTX_TREE@/uptext.h \
	@MTX_TREE@/utility.c \
	@MTX_TREE@/utility.h

## We can not use @MTX_TREE@/libp2c/p2clib.c (multiple definitions).

dist_man1_MANS = prepmx.1

## Tests
##
TESTS = m-tx.test
EXTRA_DIST += $(TESTS)
m-tx.log: prepmx$(EXEEXT)

## m-tx.test
EXTRA_DIST += tests/mozart.mtx tests/mozart.pmx
DISTCLEANFILES = mozart*

