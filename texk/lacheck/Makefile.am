## Makefile.am for the TeX Live subdirectory texk/lacheck/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = lacheck

lacheck_SOURCES = lacheck.l

nodist_man1_MANS = lacheck.1

lacheck.1: lacheck.man
	sed -e "s!%%LACHECKREV%%!Release $(REV)!" \
	    -e "s!%%LACHECKDATE%%!`date +%Y-%m-%d`!" \
	    -e "s!%%LACHECKPATH%%!lacheck!" $< > $@

EXTRA_DIST = lacheck.man

CLEANFILES = lacheck.1

dist_check_SCRIPTS = lacheck.test

TESTS = lacheck.test

EXTRA_DIST += test.tex test.out

CLEANFILES += test.xout

## Not used
##
EXTRA_DIST += lacheck.hlp make_vms.com

