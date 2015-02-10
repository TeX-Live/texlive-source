bin_PROGRAMS = fixmsxpart msxlint
fixmsxpart_SOURCES = utils.h utils.c fixmsxpart.c
msxlint_SOURCES = utils.h utils.c msxlint.c
AM_CFLAGS = -O4 -ansi -Wall -Wextra -pedantic-errors
dist_man_MANS = fixmsxpart.1 msxlint.1 
## Tests
##
TESTS = fixmsxpart.test msxlint.test

EXTRA_DIST =
EXTRA_DIST += $(TESTS)
DISTCLEANFILES =

## fixmsxpart.test
EXTRA_DIST += tests/fixtest.out tests/fixtest.tex
DISTCLEANFILES += fixtest.*
## msxlint.test
EXTRA_DIST += tests/badtest.tex tests/linttest.tex tests/linttest.out
DISTCLEANFILES += linttest.*

