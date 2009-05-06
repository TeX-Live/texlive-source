## Makefile.am for the TeX Live subdirectory texk/cjkutils/hbf2gf/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
INCLUDES = $(KPATHSEA_INCLUDES)
AM_CPPFLAGS = -Dunix -DHAVE_LIBKPATHSEA $(XX_CPPFLAGS)
AM_CFLAGS = $(XX_CFLAGS)

bin_PROGRAMS = hbf2gf

hbf2gf_SOURCES = \
	hbf.c \
	hbf.h \
	hbf2gf.c

CWEB_SRC = hbf2gf.w
CWEB_TEX = hbf2gf.tex hbf2gf.idx hbf2gf.scn
CWEB_DVI = hbf2gf.dvi

hbf2gf_DEPENDENCIES = $(KPATHSEA_DEPEND)

LDADD = $(KPATHSEA_LIBS)

## Rebuild libkpathsea
@KPATHSEA_RULE@

dist_man1_MANS = hbf2gf.1

EXTRA_DIST = $(CWEB_SRC) $(CWEB_TEX) $(CWEB_DVI) cfg

CLEANFILES = hbf2gf.log hbf2gf.toc

hbf2gf.dvi: $(CWEB_TEX)
	TEXINPUTS=.:$(srcdir):$$TEXINPUTS $(TEX) hbf2gf.tex

# Running cweave in order to create hbf2gf.tex generates in addition
# hbf2gf.idx and hbf2gf.scn; we must rerun cweave when they are not
# up to date (even if hbf2gf.tex is up to date). Thus the sub-make.
#
hbf2gf.tex hbf2gf.idx hbf2gf.scn: hbf2gf.w
	$(MAKE) $(AM_MAKEFLAGS) do-cweave
do-cweave: hbf2gf.w .FORCE
	$(CWEAVE) +ai $<
.PHONY: do-cweave .FORCE

if do_cweb
all-local: hbf2gf.dvi
hbf2gf.c: hbf2gf.w
	$(CTANGLE) $<
else
all-local:
endif

## Not used
##
EXTRA_DIST += \
	emdir.c \
	emdir.h \
	emtexdir.c \
	emtexdir.h \
	hbf2gf.def

