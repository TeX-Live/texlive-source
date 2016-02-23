## Makefile.am for the TeX Live subdirectory texk/psutils/
##
## Copyright (C) 2009-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
## We want to re-distribute the whole original psutils source tree.
EXTRA_DIST = $(PSUTILS_TREE)
## Patches applied to the original source tree
EXTRA_DIST += psutils-PATCHES

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

AM_CPPFLAGS = -I$(srcdir)/$(PSUTILS_TREE) $(KPATHSEA_INCLUDES) $(LIBPAPER_INCLUDES)
AM_CFLAGS = $(WARNING_CFLAGS)

noinst_LIBRARIES = libpsutils.a

libpsutils_a_SOURCES = \
	binary-io.h \
	paper-size.c \
	progname.c \
	progname.h \
	texlive.h \
	verror.h \
	verror.c \
	xvasprintf.h \
	@PSUTILS_TREE@/psspec.c \
	@PSUTILS_TREE@/psspec.h \
	@PSUTILS_TREE@/psutil.c \
	@PSUTILS_TREE@/psutil.h

$(libpsutils_a_OBJECTS): $(KPATHSEA_DEPEND) $(LIBPAPER_DEPEND)

bin_PROGRAMS = epsffit psbook psnup psresize psselect pstops

epsffit_SOURCES = @PSUTILS_TREE@/epsffit.c

psbook_SOURCES = @PSUTILS_TREE@/psbook.c

psnup_SOURCES = @PSUTILS_TREE@/psnup.c

psresize_SOURCES = @PSUTILS_TREE@/psresize.c

psselect_SOURCES = @PSUTILS_TREE@/psselect.c

pstops_SOURCES = @PSUTILS_TREE@/pstops.c

$(epsffit_OBJECTS) $(psbook_OBJECTS) $(psnup_OBJECTS) \
  $(psresize_OBJECTS) $(psselect_OBJECTS) $(pstops_OBJECTS): libpsutils.a

LDADD = libpsutils.a $(KPATHSEA_LIBS) $(LIBPAPER_LIBS)

perl_scripts = \
	extractres \
	includeres \
	psjoin

scriptsdir = texmf-dist/scripts/psutils
nodist_bin_SCRIPTS =

include $(srcdir)/../../am/script_links.am

texmfdir = $(datarootdir)/$(scriptsdir)
texmf_SCRIPTS = $(perl_scripts:=.pl)

install-data-hook: install-perl-links

uninstall-hook: uninstall-links

## Manpages
##
man1_MANS = \
	@PSUTILS_TREE@/epsffit.1 \
	@PSUTILS_TREE@/extractres.1 \
	@PSUTILS_TREE@/includeres.1 \
	@PSUTILS_TREE@/psbook.1 \
	@PSUTILS_TREE@/psjoin.1 \
	@PSUTILS_TREE@/psnup.1 \
	@PSUTILS_TREE@/psresize.1 \
	@PSUTILS_TREE@/psselect.1 \
	@PSUTILS_TREE@/pstops.1 \
	@PSUTILS_TREE@/psutils.1

## Data
##
configdatadir = $(datarootdir)/texmf-dist/psutils
dist_configdata_DATA = data/paper.cfg

## Rebuild libkpathsea
@KPATHSEA_RULE@
## Rebuild libpaper
@LIBPAPER_RULE@

## Tests
##
TESTS = psutils.test
psutils.log: epsffit$(EXEEXT) psbook$(EXEEXT) psnup$(EXEEXT) psresize$(EXEEXT) psselect$(EXEEXT) pstops$(EXEEXT)
EXTRA_DIST += $(TESTS)

## psutils.test
EXTRA_DIST += tests/play.ps tests/playbook.2 tests/playnup.2 tests/playnup1.2 \
	tests/playps.2 tests/playres.ps tests/playsel.2 tests/playsel1.2 \
	tests/plot.eps tests/ploth.eps tests/plotm.eps tests/plotv.eps
DISTCLEANFILES = play*.2 play*.ps plot*.eps

