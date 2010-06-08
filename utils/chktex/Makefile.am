## Makefile.am for the TeX Live subdirectory utils/chktex/
##
## Copyright (C) 2010 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
#************************************************************************
#
#  Adapted for TeX Live from chktex/Makefile.1n
#
#************************************************************************
##
ACLOCAL_AMFLAGS = -I ../../m4

## We want to re-distribute the whole original chktex source tree.
##
## With current automake (1.10.2) 'make distcheck' fails when
## DISTFILES contains a directory and files in that directory.
## Thus nodist_* for all files in $(CHKTEX_TREE).
EXTRA_DIST = $(CHKTEX_TREE)

## Patches applied to the original source tree
##
EXTRA_DIST += $(CHKTEX_TREE)-PATCHES

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`

AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = chktex

chktex_SOURCES = \
	getopt.c \
	getopt.h \
	getopt1.c \
	$(CHKTEX_TREE)/ChkTeX.c \
	$(CHKTEX_TREE)/ChkTeX.h \
	$(CHKTEX_TREE)/FindErrs.c \
	$(CHKTEX_TREE)/FindErrs.h \
	$(CHKTEX_TREE)/OpSys.c \
	$(CHKTEX_TREE)/OpSys.h \
	$(CHKTEX_TREE)/Resource.c \
	$(CHKTEX_TREE)/Resource.h \
	$(CHKTEX_TREE)/Utility.c \
	$(CHKTEX_TREE)/Utility.h \
	$(CHKTEX_TREE)/types.h

dist_man_MANS = \
	$(CHKTEX_TREE)/chktex.1 \
	$(CHKTEX_TREE)/chkweb.1 \
	$(CHKTEX_TREE)/deweb.1

dist_bin_SCRIPTS = $(CHKTEX_TREE)/chkweb
nodist_bin_SCRIPTS = deweb

# resource file
configdir = ${prefix}/texmf/chktex

config_DATA = $(CHKTEX_TREE)/chktexrc

# documentation
pdfdocdir = ${prefix}/texmf/doc/chktex

dist_pdfdoc_DATA = ChkTeX.pdf

ChkTeX.pdf: $(PDF_DEPEND)
	$(PDFLATEX) -interaction batchmode $(srcdir)/ChkTeX.tex
	$(PDFLATEX) -interaction batchmode $(srcdir)/ChkTeX.tex
CLEANFILES = ChkTeX.aux ChkTeX.log

## We distribute ChkTeX.tex (generated from $(CHKTEX_TREE)/ChkTeX.tex.in)
## and ChkTeX.pdf, but also need rules to create them if missing or
## (in maintainer-mode) to update them if required.
.PHONY: build-tex
all-local: stamp-tex
stamp-tex: ChkTeX.tmp
	$(MAKE) $(AM_MAKEFLAGS) build-tex
	$(MAKE) $(AM_MAKEFLAGS) ChkTeX.pdf
	echo timestamp >$@
DISTCLEANFILES = stamp-tex
build-tex:
	cmp -s ChkTeX.tmp $(srcdir)/ChkTeX.tex \
	  || cp ChkTeX.tmp $(srcdir)/ChkTeX.tex

EXTRA_DIST += ChkTeX.tex

# chktex tests
TESTS = chktex.test

TESTS_ENVIRONMENT = CHKTEX_TREE=$(CHKTEX_TREE)

EXTRA_DIST += chktex.test

CLEANFILES += chktest

