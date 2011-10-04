## Makefile.am for the TeX Live subdirectory texk/dvipsk/
##
## Copyright (C) 2009 - 2011 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

SUBDIRS = squeeze

INCLUDES = $(KPATHSEA_INCLUDES)
AM_CPPFLAGS = -DUNIX -DKPATHSEA -DSHIFTLOWCHARS
AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = afm2tfm dvips

afm2tfm_SOURCES = afm2tfm.c

$(afm2tfm_OBJECTS): $(KPATHSEA_DEPEND)

dvips_SOURCES = \
	bbox.c \
	color.c \
	config.h \
	debug.h \
	dopage.c \
	dosection.c \
	dospecial.c \
	download.c \
	dpicheck.c \
	drawPS.c \
	dviinput.c \
	dvips.c \
	dvips.h \
	emspecial.c \
	finclude.c \
	fontdef.c \
	header.c \
	hps.c \
	loadfont.c \
	output.c \
	papersiz.c \
	paths.h \
	pprescan.c \
	prescan.c \
	protos.h \
	protos_add.h \
	ptexmac.h \
	repack.c \
	resident.c \
	scalewidth.c \
	scanpage.c \
	search.c \
	skippage.c \
	t1part.c \
	t1part.h \
	tfmload.c \
	unpack.c \
	virtualfont.c \
	writet1.c

$(dvips_OBJECTS): $(KPATHSEA_DEPEND)

LDADD = $(KPATHSEA_LIBS)

## Rebuild libkpathsea
@KPATHSEA_RULE@

dist_man1_MANS = afm2tfm.1 dvips.1

info_TEXINFOS = dvips.texi
dvips_TEXINFOS = contrib/config.proto dvips.help

DISTCLEANFILES = $(DVIS) $(PSS)

prolog_DATA = $(prologues)

dist_prologues = \
	color.lpro \
	crop.lpro \
	finclude.lpro \
	hps.lpro \
	special.lpro \
	tex.lpro \
	texps.lpro

prologdir = ${prefix}/texmf/dvips/base

prologues = $(dist_prologues:.lpro=.pro) texc.pro

texc.lpro: texc.script tex.lpro
	$(SHELL) $(srcdir)/texc.script $(srcdir)/tex.lpro $@

SUFFIXES = .pro .lpro

# The `t[mp]-' business is so that we don't create an empty .pro file,
# if running squeeze fails for some reason.  A dependency on squeeze
# fails here, so we include it below.
.lpro.pro:
	squeeze/squeeze <$< >t-$@ && mv t-$@ $@

# When cross-compiling, EXEEXT for the build and hosts system may differ.
# Thus we depend on the auxiliary file squeeze/stamp-squeeze.
$(prologues): squeeze/stamp-squeeze

squeeze/stamp-squeeze:
	cd squeeze && $(MAKE) $(AM_MAKEFLAGS) stamp-squeeze

EXTRA_DIST = $(dist_prologues) texc.script

CLEANFILES = $(prologues) texc.lpro

## dvips tests
TESTS = afm2tfm-test.pl
TESTS += beginfontk1.test dvips.test eepic-nan.test \
         pfbincl.test quotecmd-test.pl

TESTS_ENVIRONMENT = TEXMFCNF=$(srcdir)/../kpathsea
TESTS_ENVIRONMENT += TEXCONFIG=$(srcdir)/testdata
TESTS_ENVIRONMENT += TEXFONTS=$(srcdir)/testdata
TESTS_ENVIRONMENT += TEXFONTMAPS=$(srcdir)/testdata
TESTS_ENVIRONMENT += TEXPSHEADERS=$(srcdir)/testdata

EXTRA_DIST += \
	$(TESTS) \
	testdata/8r.enc \
	testdata/ad.enc \
	testdata/alt-rule.pro \
	testdata/cmex10.pfb \
	testdata/cmex10.tfm \
	testdata/cmmi10.pfb \
	testdata/cmmi10.tfm \
	testdata/cmr10.pfb \
	testdata/cmr10.tfm \
	testdata/cmsy10.pfb \
	testdata/cmsy10.tfm \
	testdata/cmtt10.pfb \
	testdata/cmtt10.tfm \
	testdata/config.pdf \
	testdata/config.ps \
	testdata/psfonts.map \
	testdata/ptmbi.vf \
	testdata/ptmbi8r.tfm \
	testdata/ptmr.vf \
	testdata/ptmr8r.tfm \
	testdata/ptmri.vf \
	testdata/ptmri8r.tfm \
	testdata/special.pro \
	testdata/tex.pro \
	testdata/texc.pro \
	testdata/texps.pro

## afm2tfm-test.pl
DISTCLEANFILES += afmtest.tfm
## beginfontk1.test
EXTRA_DIST += testdata/beginfontk1.dvi testdata/beginfontk1.eps testdata/beginfontk1.tex
DISTCLEANFILES += beginfontk1.ps
## dvips.test
EXTRA_DIST += testdata/dvipstst.tex testdata/dvipstst.xdv testdata/dvipstst.xps
DISTCLEANFILES += dvipstst.ps missfont.log mtest.ps
## eepic-nan.test
EXTRA_DIST += testdata/eepic-nan.dvi testdata/eepic-nan.tex
DISTCLEANFILES += eepic-nan.ps
## pfbincl.test
EXTRA_DIST += testdata/pfbincl.eps testdata/pfbincl.tex testdata/pfbincl.xdv testdata/pfbincl.xps 
DISTCLEANFILES += pfbincl.ps
## quotecmd-test.pl
EXTRA_DIST += testdata/quotecmd.dvi testdata/quotecmd.tex 
DISTCLEANFILES += *badnews*

EXTRA_DIST += \
	NEWS \
	TODO \
	testdata/intoverflow.dvi \
	testdata/vfnameoverflow.dvi

## Not used
##
EXTRA_DIST += \
	atari \
	cmfonts.map \
	config.ps \
	contrib/PSTricks \
	contrib/afm-extra \
	contrib/colorsep.lpro \
	contrib/configs \
	contrib/crop.lpr \
	contrib/latex209 \
	contrib/treen.sh \
	contrib/treen2.sh \
	contrib/volker \
	contrib/whichinpath \
	cyrfonts.map \
	eufonts.map \
	flib.c \
	hps/CHANGES_HPS.txt \
	hps/README_HPS.txt \
	lafonts.map \
	makefont.c \
	mvs \
	psfonts.map \
	tex \
	vmcms \
	vms

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`
