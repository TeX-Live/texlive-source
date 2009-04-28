## Makefile.am for the TeX Live subdirectory texk/web2c/man/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
man_sources = $(bootstrap_man_sources)
EXTRA_DIST = $(bootstrap_man_sources)
bootstrap_man_sources = cweb.man tangle.man

if WEB
man_sources += $(web_man_sources)
endif WEB
EXTRA_DIST += $(web_man_sources)
web_man_sources = bibtex.man dvicopy.man dvitomp.man dvitype.man gftodvi.man \
	gftopk.man gftype.man mft.man patgen.man pktogf.man pktype.man \
	pltotf.man pooltype.man tftopl.man vftovp.man vptovf.man weave.man

man_sources += $(mpware_man_sources)
EXTRA_DIST += $(mpware_man_sources)
mpware_man_sources = dmp.man makempx.man mpto.man newer.man

if TEX
man_sources += tex.man
endif TEX
EXTRA_DIST += tex.man

if MF
man_sources += mf.man
if MFN
man_sources +=
endif MFN
endif MF
EXTRA_DIST += mf.man

if MP
man_sources += mpost.man
endif MP
EXTRA_DIST += mpost.man

if ETEX
man_sources += etex.man
endif ETEX
EXTRA_DIST += etex.man

if PDFTEX
man_sources += $(pdeftex_man_sources)
endif PDFTEX
EXTRA_DIST += $(pdeftex_man_sources)
pdeftex_man_sources = pdftex.man pdftosrc.man

if LUATEX
man_sources +=
endif LUATEX

if XETEX
man_sources +=
endif XETEX

if OTANGLE
man_sources +=
endif OTANGLE

if ALEPH
man_sources += aleph.man
endif ALEPH
EXTRA_DIST += aleph.man

man_sources += $(misc_man_sources)
EXTRA_DIST += $(misc_man_sources)
misc_man_sources = \
	amstex.man \
	eplain.man \
	latex.man \
	pdfetex.man

include $(top_srcdir)/../am/man.am

## Obsolete ?
##
EXTRA_DIST += \
	amslatex.min \
	initex.min \
	lamstex.min \
	pdftexman.m4 \
	slitex.min \
	spell.add \
	virtex.min

