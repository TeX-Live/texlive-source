## Makefile.am for the TeX Live subdirectory texk/gregorio/
##
## Copyright (C) 2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
## We want to re-distribute the whole original gregorio source tree.
EXTRA_DIST = $(GREGORIO_TREE)
## Patches applied to the original source tree
EXTRA_DIST += $(GREGORIO_TREE)-PATCHES

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

AM_CPPFLAGS = \
	-I$(top_srcdir)/$(GREGORIO_TREE)/src \
	-I$(top_srcdir)/$(GREGORIO_TREE)/src/gabc \
	-I$(top_srcdir)/$(GREGORIO_TREE)/src/dump \
	-I$(top_srcdir)/$(GREGORIO_TREE)/src/gregoriotex \
	$(KPATHSEA_INCLUDES) -D_GNU_SOURCE -DUSE_KPSE
AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = gregorio

gregorio_SOURCES = \
	@GREGORIO_TREE@/src/characters.c \
	@GREGORIO_TREE@/src/gregorio-utils.c \
	@GREGORIO_TREE@/src/messages.c \
	@GREGORIO_TREE@/src/sha1.c \
	@GREGORIO_TREE@/src/struct.c \
	@GREGORIO_TREE@/src/support.c \
	@GREGORIO_TREE@/src/unicode.c \
	@GREGORIO_TREE@/src/dump/dump.c \
	@GREGORIO_TREE@/src/gabc/gabc-elements-determination.c \
	@GREGORIO_TREE@/src/gabc/gabc-glyphs-determination.c \
	@GREGORIO_TREE@/src/gabc/gabc-notes-determination-l.c \
	@GREGORIO_TREE@/src/gabc/gabc-score-determination-l.c \
	@GREGORIO_TREE@/src/gabc/gabc-score-determination-y.c \
	@GREGORIO_TREE@/src/gabc/gabc-write.c \
	@GREGORIO_TREE@/src/gregoriotex/gregoriotex-position.c \
	@GREGORIO_TREE@/src/gregoriotex/gregoriotex-write.c \
	@GREGORIO_TREE@/src/vowel/vowel.c \
	@GREGORIO_TREE@/src/vowel/vowel-rules-l.c \
	@GREGORIO_TREE@/src/vowel/vowel-rules-y.c

LDADD = $(KPATHSEA_LIBS)

$(gregorio_OBJECTS): $(KPATHSEA_DEPEND)

## Rebuild libkpathsea
@KPATHSEA_RULE@

## TESTS
##
TESTS_ENVIRONMENT = GREGORIO_TREE='$(GREGORIO_TREE)'
TESTS = gregorio.test
gregorio.log: gregorio$(EXEEXT)
EXTRA_DIST += $(TESTS)
DISTCLEANFILES = factus.gtex veni.gtex

