## Makefile.am for axohelp
##
## Copyright (C) 2018 John Collins
## You may freely use, modify and/or distribute this file.
##

AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = axohelp
axohelp_SOURCES = axohelp.c

## Test
TESTS = axohelp.test
dist_check_SCRIPTS = $(TESTS)
CLEANFILES = test.ax2

