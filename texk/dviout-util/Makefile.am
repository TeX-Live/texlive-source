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
EXTRA_DIST += \
	tests/test.tex tests/test.dvi tests/test.spc \
	tests/testout.dvi
DISTCLEANFILES = x*test.spc x*testout.dvi x*testa.txt x*testax.dvi

## chkdvifont.test
EXTRA_DIST += tests/test.cfn
DISTCLEANFILES += test.dvi xtest.cfn

