bin_PROGRAMS = autosp rebar 
autosp_SOURCES = utils.h utils.c autosp.c 
rebar_SOURCES = utils.h utils.c rebar.c
AM_CFLAGS = -O4 -ansi -Wall -Wextra -pedantic-errors
dist_man_MANS = autosp.1 rebar.1 
## Tests
##
TESTS =  autosp.test rebar.test 

EXTRA_DIST =
EXTRA_DIST += $(TESTS)
DISTCLEANFILES =

EXTRA_DIST += tests/quod2.aspc tests/quod2.tex  tests/quod3.aspc 
DISTCLEANFILES += quod2.* quod3.*

