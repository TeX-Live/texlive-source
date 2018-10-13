## Makefile.am for the TeX Live subdirectory texk/dviout-util/
##
## Copyright (C) 2018 Japanese TeX Development Community <issue@texjp.org>
## You may freely use, modify and/or distribute this file.
##
AM_CPPFLAGS = $(PTEXENC_INCLUDES) $(KPATHSEA_INCLUDES) -DUNIX -DKPATHSEA -DPTEXENC
AM_CFLAGS = $(WARNING_CFLAGS)

$(dviout_util_OBJECTS): $(KPATHSEA_DEPEND) $(PTEXENC_DEPEND)

bin_PROGRAMS = dvispc chkdvifont

dvispc_SOURCES  = dvispc.c
chkdvifont_SOURCES = chkdvifont.c dd.h msvcdir.h

LDADD = $(KPATHSEA_LIBS)
dvispc_LDADD = $(PTEXENC_LIBS) $(LDADD)

dist_man1_MANS = dvispc.man chkdvifont.man

## Rebuild libkpathsea
@KPATHSEA_RULE@
@PTEXENC_RULE@

## Tests
##
TESTS = dvispc.test chkdvifont.test
dvispc.log: dvispc$(EXEEXT)
chkdvifont.log: chkdvifont$(EXEEXT)
EXTRA_DIST = $(TESTS)

## dvispc.test
EXTRA_DIST += tests/oldindep.spc \
	tests/oldindep.tex tests/oldindep.dvi tests/oldindepout.dvi \
	tests/reverse.tex tests/reverse.dvi tests/reverseout.dvi \
	tests/annot.tex tests/annot.dvi \
	tests/longspec.tex tests/longspec.dvi \
	tests/flatnest.tex tests/flatnest.dvi
DISTCLEANFILES = x*oldindep.spc \
	x*oldindepa.txt x*oldindepax.dvi x*oldindepout.dvi \
	x*reverseout.dvi \
	x*write.dvi x*writeout.dvi

## chkdvifont.test
EXTRA_DIST += tests/oldindep.cfn \
	tests/jis.tfm tests/jis.cfn \
	tests/eufm10.tfm tests/eufm10.cfn \
	tests/cmr10.pk tests/cmr10.cfn \
	tests/upjisr-h.vf tests/upsjir-h.cfn
DISTCLEANFILES += oldindep.dvi xoldindep.cfn \
	xjis.cfn xeufm10.cfn xeufm10.ed.cfn \
	xcmr10.cfn xupjisr-h.cfn

