## $Id$
## Makefile.am for the TeX Live subdirectory texk/texlive/
##
## Copyright 2015-2023 Karl Berry <tex-live@tug.org>
## Copyright 2009-2014 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
SUBDIRS = tl_support
if LINKED_SCRIPTS
SUBDIRS += linked_scripts
endif LINKED_SCRIPTS

## Windows wrapper and sources.
##
EXTRA_DIST = windows_wrapper

## Tests
##
TEST_EXTENSIONS = .pl
TESTS = tests/updmap-cmdline-test.pl
EXTRA_DIST += $(TESTS)
