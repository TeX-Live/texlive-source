## $Id: Makefile.am 43567 2017-03-21 17:43:10Z karl $
## Proxy Makefile.am to build pplib for TeX Live.
##
##   Copyright 2017 Karl Berry <tex-live@tug.org>
##   Copyright 2013-2015 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
## We want to re-distribute the whole Lua53 source tree.
##
EXTRA_DIST = $(PPLIB_TREE)

## Changes applied to the original source tree
##
EXTRA_DIST += TLpatches

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB) $(NEVER_NAMES_LT)

SUBDIRS = . include

AM_CPPFLAGS = -I$(top_srcdir)/$(PPLIB_TREE)/src -I$(top_srcdir)/$(PPLIB_TREE)/src/util $(ZLIB_INCLUDES) $(PPLIB_DEFINES)
AM_CFLAGS = $(WARNING_CFLAGS)  $(PPLIB_CFLAGS)

noinst_LIBRARIES = libpplib.a


nodist_libpplib_a_SOURCES = \
	@PPLIB_TREE@/src/pparray.c \
	@PPLIB_TREE@/src/ppcrypt.c \
	@PPLIB_TREE@/src/ppdict.c \
	@PPLIB_TREE@/src/ppheap.c \
	@PPLIB_TREE@/src/ppload.c \
	@PPLIB_TREE@/src/ppstream.c \
	@PPLIB_TREE@/src/pptest1.c \
	@PPLIB_TREE@/src/pptest2.c \
	@PPLIB_TREE@/src/pptest3.c \
	@PPLIB_TREE@/src/ppxref.c \
	@PPLIB_TREE@/src/util/utilbasexx.c \
	@PPLIB_TREE@/src/util/utilcrypt.c \
	@PPLIB_TREE@/src/util/utilflate.c \
	@PPLIB_TREE@/src/util/utilfpred.c \
	@PPLIB_TREE@/src/util/utiliof.c \
	@PPLIB_TREE@/src/util/utillog.c \
	@PPLIB_TREE@/src/util/utillzw.c \
	@PPLIB_TREE@/src/util/utilmd5.c \
	@PPLIB_TREE@/src/util/utilmem.c \
	@PPLIB_TREE@/src/util/utilmemheap.c \
	@PPLIB_TREE@/src/util/utilmemheapiof.c \
	@PPLIB_TREE@/src/util/utilmeminfo.c \
	@PPLIB_TREE@/src/util/utilnumber.c \
	@PPLIB_TREE@/src/util/utilsha.c 

AM_TESTS_ENVIRONMENT = PPLIB_TREE=$(PPLIB_TREE); export PPLIB_TREE;
if build
check_PROGRAMS = pptest1 pptest2 pptest3
dist_check_SCRIPTS = pplib.test
TESTS = pplib.test
endif build
pptest.log: pptest1$(EXEEXT) pptest2$(EXEEXT) pptest3$(EXEEXT)

pptest1_SOURCES = @PPLIB_TREE@/src/pptest1.c
pptest2_SOURCES = @PPLIB_TREE@/src/pptest2.c 
pptest3_SOURCES = @PPLIB_TREE@/src/pptest3.c

pptest1_CPPFLAGS = -Iinclude
pptest2_CPPFLAGS = -Iinclude
pptest3_CPPFLAGS = -Iinclude

pptest1_LDADD = libpplib.a $(ZLIB_LIBS)
pptest2_LDADD = libpplib.a $(ZLIB_LIBS)
pptest3_LDADD = libpplib.a $(ZLIB_LIBS)

LIBS = -lm


EXTRA_DIST += error.exp
CLEANFILES = error.out error.tmp


## Rebuild zlib
@ZLIB_RULE@



# Rebuild
rebuild_prereq =
rebuild_target = all

include $(srcdir)/../../am/rebuild.am
