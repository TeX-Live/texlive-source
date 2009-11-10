## Makefile.am for the TeX Live subdirectory utils/vlna/
##
## Copyright (C) 2008, 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = vlna
dist_man1_MANS = vlna.man

CWEB_SRC = vlna.w
CSMAC = csmac.tex
CWEB_TEX = vlna.tex vlna.idx vlna.scn
CWEB_DVI = vlna.dvi

DOCS = README README.en vlna.txt

EXTRA_DIST = $(CWEB_SRC) $(CSMAC) $(CWEB_TEX) $(CWEB_DVI) $(DOCS)

CLEANFILES = vlna.log vlna.toc

vlna.dvi: vlna.tex vlna.idx vlna.scn
	TEXINPUTS=.:$(srcdir):$$TEXINPUTS $(CSPLAIN) vlna.tex

# Running cweave in order to create vlna.tex generates in addition
# vlna.idx and vlna.scn; we must rerun cweave when they are not
# up to date (even if vlna.tex is up to date). Thus the sub-make.
#
vlna.tex vlna.idx vlna.scn: vlna.w
	$(MAKE) $(AM_MAKEFLAGS) do-cweave
do-cweave: vlna.w .FORCE
	$(CWEAVE) $<
.PHONY: do-cweave .FORCE

if do_cweb
all-local: vlna.dvi
vlna.c: vlna.w
	$(CTANGLE) $<
else !do_cweb
all-local:
endif !do_cweb
