## Makefile.am for the TeX Live subdirectory texk/detex/
##
## Copyright (C) 2009-2012 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

## We want to re-distribute the whole original detex source tree.
EXTRA_DIST = $(DETEX_TREE)
## Patches applied to the original source tree
EXTRA_DIST += $(DETEX_TREE)-PATCHES

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`

AM_CPPFLAGS = -I$(top_srcdir)/$(DETEX_TREE) $(KPATHSEA_INCLUDES) -DNO_MALLOC_DECL -DKPATHSEA -Dlint
AM_CFLAGS = $(WARNING_CFLAGS)

## The Makefile.in from 2002 by Thomas Esser applied the sed script
## states.sed to detex.l in order to "convert long labels to a shorter
## form so lex(1) won't overflow".  This should no longer be needed.
## Moreover, Automake distributes the `detex.c' generated from `detex.l'.

bin_PROGRAMS = detex

detex_SOURCES = \
	$(DETEX_TREE)/detex.h \
	$(DETEX_TREE)/detex.l

LDADD = $(KPATHSEA_LIBS)

$(detex_OBJECTS): $(KPATHSEA_DEPEND)

## Rebuild libkpathsea
@KPATHSEA_RULE@

man1_MANS = \
	$(DETEX_TREE)/detex.man

dist_check_SCRIPTS = detex.test

TESTS = detex.test

EXTRA_DIST += test.lout test.tex test.tout

CLEANFILES = test.out texmf.cnf

