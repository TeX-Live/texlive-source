## Makefile.am for the TeX Live subdirectory texk/texlive/
##
## Copyright (C) 2009-2014 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
SUBDIRS = tl_scripts
if LINKED_SCRIPTS
SUBDIRS += linked_scripts
endif LINKED_SCRIPTS

## Windows wrapper and sources.
##
EXTRA_DIST = w32_wrapper w64_wrapper

## Tests
##
TESTS = tests/updmap-cmdline-test.pl
EXTRA_DIST += $(TESTS)

