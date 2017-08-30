## Makefile.am for the TeX Live subdirectory utils/t1utils/
##
## Copyright (C) 2008-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
#************************************************************************
#
#  Adapted for TeX Live from t1utils/Makefile.am
#
#************************************************************************
##
## We want to re-distribute the whole original t1utils source tree.
##
## With current automake (1.10.2) 'make distcheck' fails when
## DISTFILES contains a directory and files in that directory.
## Thus nodist_* for all files in $(T1UTILS_TREE).
EXTRA_DIST = $(T1UTILS_TREE)

## Patches applied to the original source tree
##
EXTRA_DIST += t1utils-PATCHES

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

AM_CPPFLAGS = -I$(top_srcdir)/$(T1UTILS_TREE)/include
AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = t1ascii t1binary t1asm t1disasm t1unmac t1mac

dist_man_MANS = \
	$(T1UTILS_TREE)/t1ascii.1 \
	$(T1UTILS_TREE)/t1binary.1 \
	$(T1UTILS_TREE)/t1asm.1 \
	$(T1UTILS_TREE)/t1disasm.1 \
	$(T1UTILS_TREE)/t1unmac.1 \
	$(T1UTILS_TREE)/t1mac.1

t1ascii_SOURCES = \
	@T1UTILS_TREE@/clp.c \
	@T1UTILS_TREE@/t1lib.c \
	@T1UTILS_TREE@/t1ascii.c

t1binary_SOURCES = \
	@T1UTILS_TREE@/clp.c \
	@T1UTILS_TREE@/t1lib.c \
	@T1UTILS_TREE@/t1binary.c

t1asm_SOURCES = \
	@T1UTILS_TREE@/clp.c \
	@T1UTILS_TREE@/t1lib.c \
	@T1UTILS_TREE@/memmem.c \
	@T1UTILS_TREE@/t1asm.c

t1disasm_SOURCES = \
	@T1UTILS_TREE@/clp.c \
	@T1UTILS_TREE@/t1lib.c \
	@T1UTILS_TREE@/memmem.c \
	@T1UTILS_TREE@/t1disasm.c

t1unmac_SOURCES = \
	@T1UTILS_TREE@/clp.c \
	@T1UTILS_TREE@/t1lib.c \
	@T1UTILS_TREE@/t1unmac.c

t1mac_SOURCES = \
	@T1UTILS_TREE@/clp.c \
	@T1UTILS_TREE@/t1lib.c \
	@T1UTILS_TREE@/t1mac.c

LDADD = @LIBOBJS@

## Tests
##
TESTS = t1utils.test
EXTRA_DIST += $(TESTS)
t1utils.log: t1ascii$(EXEEXT) t1binary$(EXEEXT) t1asm$(EXEEXT) t1disasm$(EXEEXT)

## t1utils.test
EXTRA_DIST += tests/Symbol.pfa tests/Symbol.pfb tests/Symbol.raw
DISTCLEANFILES = Symbol.pfa Symbol.pfb Symbol.raw

