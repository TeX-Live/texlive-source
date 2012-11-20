## Makefile.am for the TeX Live subdirectory texk/xdvik/tests/
##
## Copyright (C) 2009-2012 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
include ../common.am

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/gui $(common_includes)
AM_CFLAGS = $(WARNING_CFLAGS)

check_PROGRAMS = run_tests

TESTS = $(check_PROGRAMS)

run_tests_SOURCES = \
	run_tests.c \
	run_tests.h \
	test_dl_list.c \
	test_string_list.c \
	test_string_utils.c \
	test_util.c

$(run_tests_OBJECTS): ../libxdvi.a $(common_dependencies)

LDADD = ../libxdvi.a $(common_ldadd)

../libxdvi.a: $(libxdvi_dependencies)
	cd .. && $(MAKE) $(AM_MAKEFLAGS) libxdvi.a

## Not used
##
EXTRA_DIST = test1.c

