bin_PROGRAMS = autosp rebar tex2aspc
autosp_SOURCES = utils.h utils.c autosp.c 
rebar_SOURCES = utils.h utils.c rebar.c
tex2aspc_SOURCES = utils.h utils.c tex2aspc.c
AM_CFLAGS = -O4 -ansi -Wall -Wextra -pedantic-errors
dist_man_MANS = autosp.1 rebar.1 tex2aspc.1
## Tests
##
TESTS =  autosp.test rebar.test tex2aspc.test

EXTRA_DIST =
EXTRA_DIST += $(TESTS)
DISTCLEANFILES =

EXTRA_DIST += tests/quod2.aspc tests/quod2.tex  tests/quod3.aspc tests/quod4.aspc 
DISTCLEANFILES += quod2.* quod3.* quod4.*

