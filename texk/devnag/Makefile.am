## Makefile.am for the TeX Live subdirectory texk/devnag/
##
## Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = devnag

devnag_SOURCES = src/devnag.c

## Tests.
##
TESTS = tests/devnag.test
EXTRA_DIST = $(TESTS)
## tests/devnag.test
EXTRA_DIST += \
	tests/captions.dn tests/captions.tex \
	tests/examples.dn tests/examples.tex \
	tests/misspaal.dn tests/misspaal.tex \
	tests/vedasample.dn tests/vedasample.tex

DISTCLEANFILES = captions.dn examples.dn *.tex

