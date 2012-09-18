## Makefile.am for the TeX Live subdirectory utils/psutils/
##
## Copyright (C) 2009-2012 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

## We want to re-distribute the whole original psutils source tree.
EXTRA_DIST = $(PSUTILS_TREE)
## Patches applied to the original source tree
EXTRA_DIST += $(PSUTILS_TREE)-PATCHES

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`

OS = UNIX
PAPER = a4

AM_CPPFLAGS = -DUNIX -DPAPER=\"$(PAPER)\"
AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = epsffit psbook psnup psresize psselect pstops

epsffit_SOURCES = \
	$(PSUTILS_TREE)/config.h \
	$(PSUTILS_TREE)/epsffit.c \
	$(PSUTILS_TREE)/pserror.c \
	$(PSUTILS_TREE)/pserror.h

psbook_SOURCES = \
	$(PSUTILS_TREE)/config.h \
	$(PSUTILS_TREE)/psbook.c \
	$(PSUTILS_TREE)/pserror.c \
	$(PSUTILS_TREE)/pserror.h \
	$(PSUTILS_TREE)/psutil.c \
	$(PSUTILS_TREE)/psutil.h

psnup_SOURCES = \
	$(PSUTILS_TREE)/config.h \
	$(PSUTILS_TREE)/pserror.c \
	$(PSUTILS_TREE)/pserror.h \
	$(PSUTILS_TREE)/psnup.c \
	$(PSUTILS_TREE)/psspec.c \
	$(PSUTILS_TREE)/psspec.h \
	$(PSUTILS_TREE)/psutil.c \
	$(PSUTILS_TREE)/psutil.h

psresize_SOURCES = \
	$(PSUTILS_TREE)/config.h \
	$(PSUTILS_TREE)/pserror.c \
	$(PSUTILS_TREE)/pserror.h \
	$(PSUTILS_TREE)/psresize.c \
	$(PSUTILS_TREE)/psspec.c \
	$(PSUTILS_TREE)/psspec.h \
	$(PSUTILS_TREE)/psutil.c \
	$(PSUTILS_TREE)/psutil.h

psselect_SOURCES = \
	$(PSUTILS_TREE)/config.h \
	$(PSUTILS_TREE)/pserror.c \
	$(PSUTILS_TREE)/pserror.h \
	$(PSUTILS_TREE)/psselect.c \
	$(PSUTILS_TREE)/psutil.c \
	$(PSUTILS_TREE)/psutil.h

pstops_SOURCES = \
	$(PSUTILS_TREE)/pserror.c \
	$(PSUTILS_TREE)/pserror.h \
	$(PSUTILS_TREE)/psspec.c \
	$(PSUTILS_TREE)/psspec.h \
	$(PSUTILS_TREE)/pstops.c \
	$(PSUTILS_TREE)/psutil.c \
	$(PSUTILS_TREE)/psutil.h

## Omitted fixmacps depending on removed non-free files
##   $(PSUTILS_TREE)/md{68,71}_0.ps.
perl_scripts = \
	extractres \
	fixdlsrps \
	fixfmps \
	fixpsditps \
	fixpspps \
	fixscribeps \
	fixtpps \
	fixwfwps \
	fixwpps \
	fixwwps \
	includeres \
	psmerge

sh_scripts = \
	getafm \
	showchar

scriptsdir = texmf/scripts/psutils
nodist_bin_SCRIPTS =

include $(srcdir)/../../texk/am/script_links.am

texmfdir = $(datarootdir)/$(scriptsdir)
nodist_texmf_SCRIPTS = $(perl_scripts:=.pl)
texmf_SCRIPTS = \
	$(PSUTILS_TREE)/getafm.sh \
	$(PSUTILS_TREE)/showchar.sh

install-data-hook: install-perl-links install-sh-links

uninstall-hook: uninstall-links

nodist_man1_MANS = \
	$(bin_PROGRAMS:$(EXEEXT)=.1) \
	$(perl_scripts:=.1) \
	getafm.1

CLEANFILES = $(nodist_bin_SCRIPTS) $(nodist_texmf_SCRIPTS) $(nodist_man1_MANS)

PERL = /usr/bin/env perl
MAKETEXT = $(PERL) $(srcdir)/$(PSUTILS_TREE)/maketext

%.pl: $(PSUTILS_TREE)/%.pl patchlev.h
	$(MAKETEXT) OS=$(OS) PERL="$(PERL)" $< >$@

%.1: $(PSUTILS_TREE)/%.man patchlev.h
	$(MAKETEXT) PAPER=$(PAPER) "MAN=$(nodist_man1_MANS)" $< >$@

## Tests
##
TESTS = psutils.test
EXTRA_DIST += $(TESTS)

## psutils.test
EXTRA_DIST += tests/play.ps tests/playbook.2 tests/playnup.2 tests/playps.2 \
	tests/playres.ps tests/playsel.2 \
	tests/plot.eps tests/ploth.eps tests/plotm.eps tests/plotv.eps
DISTCLEANFILES = play*.2 play*.ps plot*.eps

