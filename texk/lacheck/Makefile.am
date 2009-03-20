##
ACLOCAL_AMFLAGS = -I ../../m4

INCLUDES = $(KPATHSEA_INCLUDES)
AM_CPPFLAGS = -DKPATHSEA -DUNIX  -DNeedFunctionPrototypes

DISTCLEANFILES = detex-sed.l

bin_PROGRAMS = lacheck

lacheck_SOURCES = lacheck.l

LDADD = $(LEXLIB)

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

## Rebuild libkpathsea
@KPATHSEA_RULE@

## Not yet used
##
EXTRA_DIST += lacheck.hlp make_vms.com

