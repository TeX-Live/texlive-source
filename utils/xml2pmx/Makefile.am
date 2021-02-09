## $Id$
## Makefile.am for the TeX Live subdirectory utils/xml2pmx/
##
## Copyright 2021 Bob Tennent <rdt@queensu.org>
## You may freely use, modify and/or distribute this file.

## We want to re-distribute the whole original ps2eps source tree.
EXTRA_DIST = $(XML2PMX_TREE)

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = xml2pmx

# must use @... here or subdir $(... (literally) is created for .deps.
xml2pmx_SOURCES = \
	@XML2PMX_TREE@/Args.c \
	@XML2PMX_TREE@/Builtin.c \
	@XML2PMX_TREE@/Files.c \
	@XML2PMX_TREE@/dynlink.c \
	@XML2PMX_TREE@/exec.h \
	@XML2PMX_TREE@/gc.c \
	@XML2PMX_TREE@/image.c \
	@XML2PMX_TREE@/interp.c \
	@XML2PMX_TREE@/keiko.h \
	@XML2PMX_TREE@/loader.c \
	@XML2PMX_TREE@/obcommon.h \
	@XML2PMX_TREE@/obx.h \
	@XML2PMX_TREE@/primtab.h \
	@XML2PMX_TREE@/primtab0.c \
	@XML2PMX_TREE@/support.c \
	@XML2PMX_TREE@/wrapper.c \
	@XML2PMX_TREE@/xmain.c

man1_MANS = \
	$(XML2PMX_TREE)/xml2pmx.1 

# Tests
AM_TESTS_ENVIRONMENT = XML2PMX_TREE=$(XML2PMX_TREE); export XML2PMX_TREE;
#
TESTS = $(XML2PMX_TREE)/xml2pmx.test
EXTRA_DIST += $(TESTS)
EXTRA_DIST += tests/Telemann.xml tests/Telemann.pmx
EXTRA_DIST += configure.ac Makefile.am
xml2pmx.log: xml2pmx$(EXEEXT)
#
# xml2pmx.test files
EXTRA_DIST += $(XML2PMX_TREE)/tests

# Add action.c (included in interp.c), the test subdirectory, and
# Oberon sources from the obsrc directory.
EXTRA_DIST += action.c test \
	obsrc/b.m obsrc/Complete.m obsrc/Fifo.m obsrc/Makefile \
	obsrc/MyFiles.m obsrc/Strings1.m obsrc/Testbed.m

DISTCLEANFILES = Telemann* .deps/*
