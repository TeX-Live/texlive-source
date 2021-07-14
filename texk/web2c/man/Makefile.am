## $Id$
## Makefile.am for the TeX Live subdirectory texk/web2c/man/
## Copyright 2015-2019 Karl Berry <tex-live@tug.org>
## Copyright 2009-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
man_sources = $(bootstrap_man_sources)
EXTRA_DIST = $(bootstrap_man_sources)
bootstrap_man_sources = ctie.man cweb.man ctwill.man tangle.man tie.man

# Symlinks within $(man1dir): FILE:LINK indicates LINK.1->FILE.1
man1_links = cweb:ctangle cweb:cweave
man1_links += ctwill:ctwill-refsort ctwill:ctwill-twinx

if WEB
man_sources += $(web_man_sources)
endif WEB
EXTRA_DIST += $(web_man_sources)
web_man_sources = bibtex.man dvicopy.man dvitomp.man dvitype.man gftodvi.man \
	gftopk.man gftype.man mft.man patgen.man pktogf.man pktype.man \
	pltotf.man pooltype.man tftopl.man vftovp.man vptovf.man weave.man

if TEX
man_sources += tex.man
man1_links += tex:initex
endif TEX
EXTRA_DIST += tex.man

if MF
man_sources += mf.man
man1_links += mf:inimf
if MFN
man1_links += mf:mf-nowin
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
man_sources += $(pdftex_man_sources)
man1_links += pdftex:pdflatex
endif PDFTEX
pdftex_man_sources = pdftex.man pdftosrc.man ttf2afm.man
EXTRA_DIST += $(pdftex_man_sources)

if PTEX
man_sources += ptex.man
man1_links += ptex:eptex ptex:euptex ptex:uptex
man_sources += ppltotf.man ptftopl.man
man1_links += ppltotf:uppltotf ptftopl:uptftopl
endif PTEX
EXTRA_DIST += ptex.man ppltotf.man ptftopl.man

if LUATEX
man_sources += luatex.man
man1_links += luatex:texlua luatex:texluac \
  luatex:luahbtex luatex:luajittex \
  luatex:dviluatex
endif LUATEX
EXTRA_DIST += luatex.man

if XETEX
man_sources += xetex.man
endif XETEX
EXTRA_DIST += xetex.man

if OTANGLE
man_sources += $(otangle_man_sources)
man1_links += dvicopy:odvicopy dvitype:odvitype tangle:otangle
endif OTANGLE
EXTRA_DIST += $(otangle_man_sources)
otangle_man_sources = ofm2opl.man opl2ofm.man ovf2ovp.man ovp2ovf.man

if ALEPH
man_sources += aleph.man
endif ALEPH
EXTRA_DIST += aleph.man

man_sources += $(misc_man_sources)
EXTRA_DIST += $(misc_man_sources)
misc_man_sources = \
	amstex.man \
	latex.man \
	latex-dev.man \
	pdfetex.man

man1_links += latex-dev:pdflatex-dev \
  latex-dev:xelatex-dev \
  latex-dev:lualatex-dev \
  latex-dev:dvilualatex-dev \
  latex-dev:platex-dev \
  latex-dev:uplatex-dev

include $(top_srcdir)/../../am/man.am

include $(top_srcdir)/../../am/man1_links.am

install-data-hook: install-man1-links

uninstall-hook: uninstall-man1-links

EXTRA_DIST += \
	spell.add
