## $Id$
## Makefile.am for the TeX Live subdirectory texk/dvi2tty/
##
## Copyright 2017 Karl Berry <tex-live@tug.org>
## Copyright 2009-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.

## We want to re-distribute the whole original dvi2tty source tree.
EXTRA_DIST = $(DVI2TTY_TREE)

## Patches applied to the original source tree
EXTRA_DIST += TLpatches

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

AM_CPPFLAGS = $(PTEXENC_INCLUDES) $(KPATHSEA_INCLUDES) -DKPATHSEA -DPTEXENC
AM_CFLAGS = $(WARNING_CFLAGS)

LDADD = $(KPATHSEA_LIBS)

bin_PROGRAMS = disdvi dvi2tty

disdvi_SOURCES = \
	@DVI2TTY_TREE@/commands.h \
	@DVI2TTY_TREE@/disdvi.c

$(disdvi_OBJECTS): $(KPATHSEA_DEPEND) $(PTEXENC_DEPEND)

disdvi_LDADD = $(PTEXENC_LIBS) $(LDADD)

dvi2tty_SOURCES = \
	@DVI2TTY_TREE@/commands.h \
	@DVI2TTY_TREE@/dvi2tty.c \
	@DVI2TTY_TREE@/dvi2tty.h \
	@DVI2TTY_TREE@/tex2ucs.h \
	@DVI2TTY_TREE@/dvistuff.c

$(dvi2tty_OBJECTS): $(KPATHSEA_DEPEND) $(PTEXENC_DEPEND)

dvi2tty_LDADD = $(PTEXENC_LIBS) $(LDADD)

man1_MANS = \
	$(DVI2TTY_TREE)/disdvi.1 \
	$(DVI2TTY_TREE)/dvi2tty.1

## Rebuild libkpathsea & libptexenc
@KPATHSEA_RULE@
@PTEXENC_RULE@

## Tests.
##
TESTS = disdvi.test dvi2tty.test
disdvi.log: disdvi$(EXEXET)
dvi2tty.log: dvi2tty$(EXEEXT)

AM_TESTS_ENVIRONMENT = DVI2TTY_TREE=$(DVI2TTY_TREE); export DVI2TTY_TREE;
EXTRA_DIST += $(TESTS)

## disdvi.test
EXTRA_DIST += output/ligtest.dis
EXTRA_DIST += output/ptex1.dis output/uptex1.dis output/ptex2Eu.dis output/uptex2Eu.dis
DISTCLEANFILES = ligtest.dout
DISTCLEANFILES += ptex1.dis uptex1.dis ptex2Eu.dis uptex2Eu.dis
## dvi2tty.test
EXTRA_DIST += output/test.tty output/testc.tty output/tests.tty output/testu.tty output/testEu.tty
EXTRA_DIST += output/ligtest.tty output/ligtestu.tty output/ligtestEu.tty
EXTRA_DIST += output/table.tty output/tablec.tty output/tables.tty output/tableu.tty output/tableuC.tty \
	output/tableEu.tty output/tableEu1.tty output/tableEuC.tty output/tableEua.tty
EXTRA_DIST += output/ptex1Eu.tty output/uptex1.tty output/fntdef0.tty
DISTCLEANFILES += test.out testc.out tests.out testu.out testEu.out
DISTCLEANFILES += ligtest.out ligtestu.out ligtestEu.out
DISTCLEANFILES += table.out tablec.out tables.out tableu.out tableuC.out \
	tableEu.out tableEu1.out tableEuC.out tableEua.out
DISTCLEANFILES += ptex1Eu.out ptex1EuJ.out uptex1.out uptex1J.out
DISTCLEANFILES += fntdef0.out fntdef1.out fntdef2.out fntdef3.out

