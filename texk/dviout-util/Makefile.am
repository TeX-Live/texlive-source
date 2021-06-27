## Makefile.am for the TeX Live subdirectory texk/dviout-util/
##
## Copyright (C) 2018-2021 Japanese TeX Development Community <issue@texjp.org>
## You may freely use, modify and/or distribute this file.
##
AM_CPPFLAGS = $(PTEXENC_INCLUDES) $(KPATHSEA_INCLUDES) -DUNIX -DKPATHSEA -DPTEXENC
AM_CFLAGS = $(WARNING_CFLAGS)

$(dviout_util_OBJECTS): $(KPATHSEA_DEPEND) $(PTEXENC_DEPEND)

bin_PROGRAMS = dvispc chkdvifont

dvispc_SOURCES  = dvispc.c common.h
chkdvifont_SOURCES = chkdvifont.c dd.h msvcdir.h common.h

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
	tests/oldindep.tex tests/oldindep.dvi \
	tests/oldindep.dry tests/oldindepout.dvi \
	tests/under.tex tests/under.dvi \
	tests/underout.dvi \
	tests/tpicpn.tex tests/tpicpn.dvi \
	tests/tpicpn.dry tests/tpicpnout.dvi \
	tests/tpicsh.tex tests/tpicsh.dvi \
	tests/tpicsh.dry \
	tests/reverse.tex tests/reverse.dvi \
	tests/reverse.dry tests/reverseout.dvi \
	tests/first.tex tests/first.dvi \
	tests/first.dry tests/firstout.dvi \
	tests/firstpn.tex tests/firstpn.dvi \
	tests/firstpn.dry \
	tests/annot.tex tests/annot.dvi \
	tests/longspec.tex tests/longspec.dvi \
	tests/flatnest.tex tests/flatnest.dvi \
	tests/jisx0208.dvi tests/jisx0208.txt \
	tests/putj.dump.dvi tests/putj.dump.txt \
	tests/put.dump.dvi tests/put.dump.txt
DISTCLEANFILES = x*oldindep.spc \
	x*oldindepa.txt x*oldindepax.dvi \
	x*oldindep.dry x*oldindepout.dvi \
	x*underout.dvi \
	x*tpicpn.dry x*tpicpnout.dvi \
	x*tpicsh.dry \
	x*reverse.dry x*reverseout.dvi \
	x*first.dry x*firstout.dvi \
	x*firstpn.dry \
	x*write.dvi x*writeout.dvi \
	*.da.txt *.da.dvi

## chkdvifont.test
EXTRA_DIST += tests/oldindep.cfn \
	tests/jis.tfm tests/jis.cfn \
	tests/eufm10.tfm tests/eufm10.cfn \
	tests/cmr10.pk tests/cmr10.cfn \
	tests/upjisr-h.vf tests/upsjir-h.cfn \
	tests/uparse.tfm tests/uparse.cfn \
	tests/skipjfmp.tfm tests/skipjfmp.cfn \
	tests/upphirakakuw3-h.tfm tests/upphirakakuw3-h.cfn \
	tests/upphirakakuw6-h.tfm tests/upphirakakuw6-h.cfn
DISTCLEANFILES += oldindep.dvi xoldindep.cfn \
	xjis.cfn xeufm10.cfn xeufm10.ed.cfn \
	xcmr10.cfn xupjisr-h.cfn \
	xuparse.cfn xskipjfmp.cfn \
	xupp3-h.cfn xupp6-h.cfn

