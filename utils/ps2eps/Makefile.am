## Makefile.am for the TeX Live subdirectory utils/ps2eps/
##
## Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

## We want to re-distribute the whole original ps2eps source tree.
EXTRA_DIST = $(PS2EPS_TREE)
## Patches applied to the original source tree
EXTRA_DIST += $(PS2EPS_TREE)-PATCHES

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn -o -name .deps -o -name .dirstamp -o -name '*.$(OBJEXT)'`

AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = bbox

bbox_SOURCES = \
	@PS2EPS_TREE@/src/C/bbox.c

man1_MANS = \
	$(PS2EPS_TREE)/doc/man/man1/bbox.1 \
	$(PS2EPS_TREE)/doc/man/man1/ps2eps.1

perl_scripts = ps2eps
scriptsdir = texmf-dist/scripts/ps2eps
nodist_bin_SCRIPTS =

include $(srcdir)/../../am/script_links.am

texmfdir = $(datarootdir)/$(scriptsdir)
nodist_texmf_SCRIPTS = $(PS2EPS_TREE)/bin/ps2eps.pl

install-data-hook: install-perl-links
uninstall-hook: uninstall-links

CLEANFILES = $(nodist_bin_SCRIPTS)

## Tests
##
TESTS = bbox.test
EXTRA_DIST += $(TESTS)

## bbox.test
EXTRA_DIST += tests/plot.pbm tests/plot.out
DISTCLEANFILES = plot.out

