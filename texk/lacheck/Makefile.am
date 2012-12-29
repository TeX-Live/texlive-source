## Makefile.am for the TeX Live subdirectory texk/lacheck/
##
## Copyright (C) 2009-2012 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

LACHECK_TREE = lacheck-$(REV)

## We want to re-distribute the whole original lacheck source tree.
EXTRA_DIST = $(LACHECK_TREE)
## Patches applied to the original source tree
EXTRA_DIST += $(LACHECK_TREE)-PATCHES

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`

AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = lacheck

lacheck_SOURCES = $(LACHECK_TREE)/lacheck.l

nodist_man1_MANS = lacheck.1

lacheck.1: $(LACHECK_TREE)/lacheck.man
	$(AM_V_GEN)sed -e "s!%%LACHECKREV%%!Release $(REV)!" \
	    -e "s!%%LACHECKPATH%%!lacheck!" $< > $@

CLEANFILES = lacheck.1

dist_check_SCRIPTS = lacheck.test

TESTS = lacheck.test
TESTS_ENVIRONMENT = LACHECK_TREE=$(LACHECK_TREE)

CLEANFILES += test.out

