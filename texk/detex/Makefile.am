## $Id$
## Makefile.am for the TeX Live subdirectory texk/detex/
##
## Copyright 2017-2022 Karl Berry <tex-live@tug.org>
## Copyright 2009-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
## We want to re-distribute the whole original detex source tree.
EXTRA_DIST = $(DETEX_TREE)
## Patches applied to the original source tree
EXTRA_DIST += TLpatches

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

AM_CPPFLAGS = -I$(top_srcdir)/$(DETEX_TREE) $(KPATHSEA_INCLUDES) -DNO_MALLOC_DECL -DKPATHSEA -Dlint
AM_CFLAGS = $(WARNING_CFLAGS)

## The Makefile.in from 2002 by Thomas Esser applied the sed script
## states.sed to detex.l in order to "convert long labels to a shorter
## form so lex(1) won't overflow".  This should no longer be needed.
## Moreover, Automake distributes the `detex.c' generated from `detex.l'.

bin_PROGRAMS = detex

detex_SOURCES = \
	@DETEX_TREE@/detex.h \
	@DETEX_TREE@/detex.l

LDADD = $(KPATHSEA_LIBS)

$(detex_OBJECTS): $(KPATHSEA_DEPEND)

## Rebuild libkpathsea
@KPATHSEA_RULE@

man1_MANS = \
	$(DETEX_TREE)/detex.man

dist_check_SCRIPTS = detex.test

TESTS = detex.test
detex.log: detex$(EXEEXT)

EXTRA_DIST += test.lout test.tex test.tout testverb.tex

CLEANFILES = test.out texmf.cnf

