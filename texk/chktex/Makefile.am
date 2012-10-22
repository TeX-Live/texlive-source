## Makefile.am for the TeX Live subdirectory texk/chktex/
##
## Copyright (C) 2010-2012 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
#************************************************************************
#
#  Adapted for TeX Live from chktex/Makefile.in
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

AM_CPPFLAGS = $(REGEX_INCLUDES) $(KPATHSEA_INCLUDES) -DKPATHSEA
AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = chktex

chktex_SOURCES = \
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
scriptsdir = texmf/scripts/chktex
nodist_bin_SCRIPTS =

include $(srcdir)/../../texk/am/script_links.am

texmfdir = $(datarootdir)/$(scriptsdir)
nodist_texmf_SCRIPTS = $(perl_scripts:=.pl)
texmf_SCRIPTS = $(CHKTEX_TREE)/chkweb.sh

install-data-hook: install-perl-links install-sh-links
uninstall-hook: uninstall-links

# resource file
configdir = $(datarootdir)/texmf/chktex

config_DATA = $(CHKTEX_TREE)/chktexrc

# documentation
pdfdocdir = $(datarootdir)/texmf/doc/chktex

dist_pdfdoc_DATA = $(CHKTEX_TREE)/ChkTeX.pdf

CLEANFILES = $(nodist_bin_SCRIPTS)

## Rebuild libkpathsea
@KPATHSEA_RULE@

## Tests
##
TESTS = chktex.test
EXTRA_DIST += $(TESTS)
TESTS_ENVIRONMENT = CHKTEX_TREE=$(CHKTEX_TREE) TEXMFCNF=$(abs_srcdir)/../kpathsea

CLEANFILES += chktest

