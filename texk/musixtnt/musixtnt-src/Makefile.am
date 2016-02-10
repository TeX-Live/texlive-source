bin_PROGRAMS = msxlint
msxlint_SOURCES = utils.h utils.c msxlint.c
AM_CFLAGS = -O4 -ansi -Wall -Wextra -pedantic-errors
dist_man_MANS =  msxlint.1 
## Tests
##
TESTS =  msxlint.test

EXTRA_DIST =
EXTRA_DIST += $(TESTS)
DISTCLEANFILES =

## msxlint.test
EXTRA_DIST += tests/badtest.tex tests/linttest.tex tests/linttest.out
DISTCLEANFILES += linttest.*

