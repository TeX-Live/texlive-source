## $Id$
## Makefile.am for axohelp (in TeX Live)
##
## Copyright (C) 2018 John Collins
## You may freely use, modify and/or distribute this file.
##

## We want to re-distribute the whole original ps2eps source tree.
EXTRA_DIST = $(AXODRAW2_TREE)

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = axohelp
# must use @... for _SOURCES or subdir $(... (literally) is created for .deps.
axohelp_SOURCES = @AXODRAW2_TREE@/axohelp.c

man1_MANS = $(AXODRAW2_TREE)/axohelp.1

## Test
AM_TESTS_ENVIRONMENT = AXODRAW2_TREE=$(AXODRAW2_TREE); export AXODRAW2_TREE;
#
TESTS = $(AXODRAW2_TREE)/axohelp.test
EXTRA_DIST += $(TESTS)
axohelp.log: axohelp$(EXEEXT)
#
## test files.
EXTRA_DIST += \
  $(AXODRAW2_TREE)/test.ax1 \
  $(AXODRAW2_TREE)/test.ax2-std \
  $(AXODRAW2_TREE)/example.tex

dist_check_SCRIPTS = $(TESTS)
CLEANFILES = test.ax2

