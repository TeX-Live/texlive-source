## $Id$
## Makefile.am for the TeX Live subdirectory texk/chktex/
##
## Copyright 2017-2022 Karl Berry <tex-live@tug.org>
## Copyright 2010-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
#************************************************************************
#
#  Adapted for TeX Live from chktex/Makefile.in
#
#************************************************************************

## We want to re-distribute the whole original chktex source tree.
##
## With current automake (1.10.2) 'make distcheck' fails when
## DISTFILES contains a directory and files in that directory.
## Thus nodist_* for all files in $(CHKTEX_TREE).
EXTRA_DIST = $(CHKTEX_TREE)

## Patches applied to the original source tree
EXTRA_DIST += TLpatches

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

AM_CPPFLAGS = $(REGEX_INCLUDES) $(KPATHSEA_INCLUDES) -DKPATHSEA
AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = chktex

chktex_SOURCES = \
	@CHKTEX_TREE@/ChkTeX.c \
	@CHKTEX_TREE@/ChkTeX.h \
	@CHKTEX_TREE@/FindErrs.c \
	@CHKTEX_TREE@/FindErrs.h \
	@CHKTEX_TREE@/OpSys.c \
	@CHKTEX_TREE@/OpSys.h \
	@CHKTEX_TREE@/Resource.c \
	@CHKTEX_TREE@/Resource.h \
	@CHKTEX_TREE@/Utility.c \
	@CHKTEX_TREE@/Utility.h \
	@CHKTEX_TREE@/types.h

if MINGW32
REGEX_INCLUDES = -I$(srcdir)/regex
chktex_SOURCES += \
	regex/regex.c \
	regex/regex.h
endif MINGW32

## included by regex/regex.c
EXTRA_chktex_SOURCES = \
	regex/regcomp.c \
	regex/regex_internal.c \
	regex/regex_internal.h \
	regex/regexec.c

EXTRA_DIST += \
	regex/COPYING.LIB \
	regex/README

$(chktex_OBJECTS): $(KPATHSEA_DEPEND)

LDADD = $(KPATHSEA_LIBS)

man_MANS = \
	$(CHKTEX_TREE)/chktex.1 \
	$(CHKTEX_TREE)/chkweb.1 \
	$(CHKTEX_TREE)/deweb.1

# scripts
perl_scripts = deweb
sh_scripts = chkweb
scriptsdir = texmf-dist/scripts/chktex
nodist_bin_SCRIPTS =

include $(srcdir)/../../am/script_links.am

texmfdir = $(datarootdir)/$(scriptsdir)
nodist_texmf_SCRIPTS = $(perl_scripts:=.pl)
texmf_SCRIPTS = $(CHKTEX_TREE)/chkweb.sh

install-data-hook: install-perl-links install-sh-links
uninstall-hook: uninstall-links

# resource file
configdir = $(datarootdir)/texmf-dist/chktex

config_DATA = $(CHKTEX_TREE)/../chktexrc

# documentation
pdfdocdir = $(datarootdir)/texmf-dist/doc/chktex

pdfdoc_DATA = ChkTeX.pdf

if MAINTAINER_MODE
ChkTeX.pdf: ChkTeX.tex
	$(LATEX) ChkTeX.tex
	$(DVIPS) -o ChkTeX.ps ChkTeX.dvi
	$(PS2PDF) ChkTeX.ps $@
ChkTeX.tex: stamp-ChkTeX
stamp-ChkTeX: ChkTeX.t
	@if cmp -s $(srcdir)/ChkTeX.tex ChkTeX.t 2>/dev/null; then \
	echo "ChkTeX.tex is unchanged"; \
	else \
	  echo "cp ChkTeX.t ChkTeX.tex"; \
	  cp ChkTeX.t ChkTeX.tex; \
	fi
	date >$@
endif MAINTAINER_MODE

EXTRA_DIST += ChkTeX.pdf ChkTeX.tex
CLEANFILES = $(nodist_bin_SCRIPTS) ChkTeX.aux ChkTeX.dvi ChkTeX.log ChkTeX.ps stamp-ChkTeX

## Rebuild libkpathsea
@KPATHSEA_RULE@

## Tests
TESTS = chktex.test
chktex.log: chktex$(EXEEXT)
EXTRA_DIST += $(TESTS)
AM_TESTS_ENVIRONMENT  = CHKTEX_TREE=$(CHKTEX_TREE); export CHKTEX_TREE;
AM_TESTS_ENVIRONMENT += TEXMFCNF=$(abs_srcdir)/../kpathsea; export TEXMFCNF;

CLEANFILES += chktest
