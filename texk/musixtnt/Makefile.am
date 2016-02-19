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
EXTRA_DIST += musixtnt-PATCHES

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

AM_CPPFLAGS = -I$(top_srcdir)/$(MUSIXTNT_TREE) $(KPATHSEA_INCLUDES) -DKPATHSEA
AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = msxlint

msxlint_SOURCES = \
	@MUSIXTNT_TREE@/msxlint.c \
	@MUSIXTNT_TREE@/utils.c

$(msxlint_OBJECTS): $(KPATHSEA_DEPEND)

LDADD = $(KPATHSEA_LIBS)

dist_man_MANS = \
	$(MUSIXTNT_TREE)/msxlint.1

## Rebuild libkpathsea
@KPATHSEA_RULE@

## Tests
##
TESTS_ENVIRONMENT = testsrc="$(srcdir)/$(MUSIXTNT_TREE)/tests"
TESTS = $(MUSIXTNT_TREE)/msxlint.test
$(MUSIXTNT_TREE)/msxlint.log: msxlint$(EXEEXT)
DISTCLEANFILES = linttest.*
NEVER_DIST += $(TESTS:.test=.log) $(TESTS:.test=.trs)

