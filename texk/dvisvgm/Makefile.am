## $Id$
# Makefile.am for the TeX Live subdirectory texk/dvisvgm/
#
# Copyright 2015-2020 Karl Berry <tex-live@tug.org>
# Copyright 2009-2015 Peter Breitenlohner <tex-live@tug.org>
# You may freely use, modify and/or distribute this file.
#
# Adapted for TeX Live from original dvisvgm Makefile.am
# Copyright 2005-2017 Martin Gieseking

## Build in the original source setup.
SUBDIRS = . $(DVISVGM_TREE)
DIST_SUBDIRS = $(SUBDIRS)

## We want to re-distribute the whole original dvisvgm source tree.
EXTRA_DIST = $(DVISVGM_TREE)

## Patches applied to the original source tree
EXTRA_DIST += TLpatches

# We want to install the man page, but the upstream doc/Makefile.am
# does lots of other stuff.
man_MANS = $(dvisvgm_srcdir)/doc/dvisvgm.1

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

dvisvgm_dependencies = $(KPATHSEA_DEPEND) $(ZLIB_DEPEND) $(FREETYPE2_DEPEND) $(POTRACE_DEPEND)
#
config.force: $(dvisvgm_dependencies)
	echo timestamp >config.force
	$(SHELL) ./config.status --recheck
#
DISTCLEANFILES = config.force CXXLD.sh

## Tests (our trivial one and additions, not the original full test suite).
##
TESTS = tests/dvisvgm.test tests/dvisvgm-sample.test tests/dvisvgm-uptex.test tests/dvisvgm-hara.test
tests/dvisvgm.log: $(DVISVGM_TREE)/src/dvisvgm$(EXEEXT)
tests/dvisvgm-sample.log: $(DVISVGM_TREE)/src/dvisvgm$(EXEEXT)
tests/dvisvgm-uptex.log: $(DVISVGM_TREE)/src/dvisvgm$(EXEEXT)
tests/dvisvgm-hara.log: $(DVISVGM_TREE)/src/dvisvgm$(EXEEXT)
EXTRA_DIST += $(TESTS)

## dvisvgm-sample.test
EXTRA_DIST += tests/sample_v2-nf.svg tests/sample_v2-wf.svg \
	tests/sample_v3-nf.svg tests/sample_v3-wf.svg
DISTCLEANFILES += sample*.svg

## dvisvgm-uptex.test, dvisvgm-hara.test
EXTRA_DIST += tests/upjf.dvi tests/upjf.tfm tests/upjv.tfm \
	tests/upjf-r.tfm tests/upjf-g.tfm tests/upjv-r.tfm tests/upjv-g.tfm \
	tests/upjf-r00.ofm tests/upjf-g00.ofm tests/upjv-r00.ofm tests/upjv-g00.ofm \
	tests/upjf_full.vf tests/upjf_omit.vf tests/upjv_full.vf tests/upjv_omit.vf \
	tests/upjf1_full.vf tests/upjf1_omit.vf \
	tests/upjf.svg tests/upjf1.svg tests/upjf.map tests/upjf-hara.map
DISTCLEANFILES += upjf*.svg
