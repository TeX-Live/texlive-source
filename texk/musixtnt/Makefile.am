## Makefile.am for the TeX Live subdirectory texk/musixtnt/
##
## Copyright (C) 2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
## We want to re-distribute the whole musixtnt source tree.
##
EXTRA_DIST = $(MUSIXTNT_TREE)

## Changes applied to the original source tree
##
EXTRA_DIST += $(MUSIXTNT_TREE)-PATCHES

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

AM_CPPFLAGS = -I$(top_srcdir)/$(MUSIXTNT_TREE) $(KPATHSEA_INCLUDES) -DKPATHSEA
AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = fixmsxpart msxlint

fixmsxpart_SOURCES = \
	@MUSIXTNT_TREE@/fixmsxpart.c \
	@MUSIXTNT_TREE@/utils.c

msxlint_SOURCES = \
	@MUSIXTNT_TREE@/msxlint.c \
	@MUSIXTNT_TREE@/utils.c

$(fixmsxpart_OBJECTS) $(msxlint_OBJECTS): $(KPATHSEA_DEPEND)

LDADD = $(KPATHSEA_LIBS)

dist_man_MANS = \
	$(MUSIXTNT_TREE)/fixmsxpart.1 \
	$(MUSIXTNT_TREE)/msxlint.1

## Rebuild libkpathsea
@KPATHSEA_RULE@

## Tests
##
TESTS_ENVIRONMENT = testsrc="$(srcdir)/$(MUSIXTNT_TREE)/tests"
TESTS = $(MUSIXTNT_TREE)/fixmsxpart.test $(MUSIXTNT_TREE)/msxlint.test
DISTCLEANFILES = fixtest.* linttest.*
NEVER_DIST += $(TESTS:.test=.log) $(TESTS:.test=.trs)

