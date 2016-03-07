## $Id$
## Makefile.am for the TeX Live subdirectory utils/autosp/
##
## Copyright 2016 Karl Berry <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
## We want to re-distribute the whole original ps2eps source tree.
EXTRA_DIST = $(AUTOSP_TREE)

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

AM_CFLAGS = $(WARNING_CFLAGS)

# We intentionally omit the rebar program, since the name just seems to
# be generic to end up in system bindirs, and it's said to just be a
# convenience.  People can get it from the Werner Icking Music Archive.
bin_PROGRAMS = autosp

# must use @... here or subdir $(... (literally) is created for .deps.
autosp_SOURCES = \
	@AUTOSP_TREE@/autosp.c \
	@AUTOSP_TREE@/utils.c

man1_MANS = \
	$(AUTOSP_TREE)/autosp.1

## Tests

# We want to use the test and tests/ tree from the original dist.
# So as to avoid embedding the original dist dir name in the test
# script, we use $test_srcdir, defined here.
AM_TESTS_ENVIRONMENT = test_srcdir=$(srcdir)/$(AUTOSP_TREE)/tests;

TESTS = $(AUTOSP_TREE)/autosp.test
EXTRA_DIST += $(TESTS)
autosp.log: autosp$(EXEEXT)

## autosp.test files
EXTRA_DIST += $(AUTOSP_TREE)/tests


