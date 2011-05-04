## Makefile.am for the TeX Live subdirectory utils/vlna/
##
## Copyright (C) 2008-2011 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

## We want to re-distribute the whole original vlna source tree.
EXTRA_DIST = $(VLNA_TREE)
## Patches applied to the original source tree
EXTRA_DIST += $(VLNA_TREE)-PATCHES

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`

AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = vlna
man1_MANS = $(VLNA_TREE)/vlna.man

EXTRA_DIST += vlna.dvi vlna.tex vlna.idx vlna.scn

CLEANFILES = vlna.log vlna.toc

vlna.dvi: vlna.tex vlna.idx vlna.scn
	TEXINPUTS=.:$(srcdir)/$(VLNA_TREE):$$TEXINPUTS $(CSPLAIN) vlna.tex

# Running cweave in order to create vlna.tex generates in addition
# vlna.idx and vlna.scn; we must rerun cweave when they are not
# up to date (even if vlna.tex is up to date). Thus the sub-make.
#
vlna.tex vlna.idx vlna.scn: $(VLNA_TREE)/vlna.w
	$(MAKE) $(AM_MAKEFLAGS) do-cweave
do-cweave: $(VLNA_TREE)/vlna.w .FORCE
	$(CWEAVE) $<
.PHONY: do-cweave .FORCE

if do_cweb
all-local: vlna.dvi
vlna.c: $(VLNA_TREE)/vlna.w
	$(CTANGLE) $<
else !do_cweb
all-local:
endif !do_cweb
