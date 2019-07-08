## $Id$
## Makefile.am for the TeX Live subdirectory utils/lacheck/
##
## Copyright 2015-2019 Karl Berry <tex-live@tug.org>
## Copyright 2009-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.

AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = lacheck

lacheck_SOURCES = lacheck.l

## Manpage
nodist_man1_MANS = lacheck.1

## Test
TESTS = lacheck.test test-def.test
dist_check_SCRIPTS = $(TESTS)
lacheck.log: lacheck$(EXEEXT)
EXTRA_DIST = test.old test.tex test-def.tex test-def.xout
CLEANFILES = test.out test-def.out

## Not used
EXTRA_DIST += lacheck.hlp make_vms.com

