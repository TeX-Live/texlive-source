## Makefile.am for the TeX Live subdirectory texk/detex/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

EXTRA_DIST = detex-2.7 detex-2.8 detex-2.8-PATCHES

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`

INCLUDES = $(KPATHSEA_INCLUDES)
AM_CPPFLAGS = -DKPATHSEA -Dlint
AM_CFLAGS = $(WARNING_CFLAGS)

## The Makefile.in from 2002 by Thomas Esser applied the sed script
## states.sed to detex.l in order to "convert long labels to a shorter
## form so lex(1) won't overflow".  This should no longer be needed.
## Moreover, Automake distributes the `detex.c' generated from `detex.l'.

bin_PROGRAMS = detex

detex_SOURCES = detex.h detex.l

LDADD = $(KPATHSEA_LIBS)

$(detex_OBJECTS): $(KPATHSEA_DEPEND)

## Rebuild libkpathsea
@KPATHSEA_RULE@

dist_man1_MANS = detex.man

dist_check_SCRIPTS = detex.test

TESTS = detex.test

EXTRA_DIST += test.lout test.tex test.tout

CLEANFILES = test.out texmf.cnf

