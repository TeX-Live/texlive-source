## $Id$
## Makefile.am for the TeX Live subdirectory texk/lcdf-typetools/
##
## Copyright 2017 Karl Berry <tex-live@tug.org>
## Copyright 2009-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
#  Adapted for TeX Live from lcdf-typetools-src/Makefile.am
#  Copyright (c) 2003-2011 Eddie Kohler

## We use the original lcdf-typetools source tree.
SUBDIRS = . $(LCDF_TYPETOOLS_TREE)
DIST_SUBDIRS = $(SUBDIRS)

## Patches applied to the original source tree
EXTRA_DIST = TLpatches

## We must explicitly distribute some top-level files.
EXTRA_DIST += \
	$(LCDF_TYPETOOLS_TREE)/autoconf.h.in \
	$(LCDF_TYPETOOLS_TREE)/configure.ac \
	$(LCDF_TYPETOOLS_TREE)/m4

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am

all-local: config.force

config.force: $(KPATHSEA_DEPEND)
	echo timestamp >config.force
	$(SHELL) ./config.status --recheck

DISTCLEANFILES = config.force CXXLD.sh

## Tests
AM_TESTS_ENVIRONMENT = LCDF_TYPETOOLS_TREE=$(LCDF_TYPETOOLS_TREE); export LCDF_TYPETOOLS_TREE;

TESTS = tests/t1.test tests/tfm.test tests/ttf.test
EXTRA_DIST += $(TESTS)

## tests/t1.test
EXTRA_DIST += tests/t2c.enc tests/uhvrrc8a.pfb \
	tests/uhv_dl.pfa tests/uhv_dl.pfb tests/uhv_dl.ps \
	tests/uhv_re.afm tests/uhv_re.pfa
DISTCLEANFILES += uhv*
## tests/tfm.test
EXTRA_DIST += tests/antpolt-regular.otf tests/texnansx.enc \
	tests/Ant.enc tests/Ant.map tests/Ant.pfb tests/Ant.pl
DISTCLEANFILES += Ant* a_enhg3c.enc
## tests/ttf.test
EXTRA_DIST += tests/FonetikaDaniaIwonaeBold.ttf tests/texmfhome.otf \
	tests/Fon.post tests/Fon.t42 \
	tests/tmf.info tests/tmf.pfa tests/tmf.pfb tests/tmf.tables
DISTCLEANFILES += Fon* tmf.*
