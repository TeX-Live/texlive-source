## Makefile.am for the TeX Live subdirectory texk/gsftopk/
##
## Copyright (C) 2009-2014 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
AM_CPPFLAGS = $(KPATHSEA_INCLUDES) -DKPATHSEA -Dlint
AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = gsftopk

gsftopk_SOURCES = \
	gsftopk.c \
	version.h

$(gsftopk_OBJECTS): config.force

config.force: $(KPATHSEA_DEPEND)
	echo timestamp >config.force
	$(SHELL) ./config.status --recheck

DISTCLEANFILES = config.force

LDADD = $(KPATHSEA_LIBS)

## Rebuild libkpathsea
@KPATHSEA_RULE@

dist_gsftopkpsheader_DATA = render.ps

gsftopkpsheaderdir = $(datarootdir)/texmf-dist/dvips/gsftopk

nodist_man1_MANS = gsftopk.1

gsftopk.1: gsftopk1.sed sedscript
	$(AM_V_GEN)sed -f sedscript <$(srcdir)/gsftopk1.sed >$@

sedscript: mksedscript Makefile
	$(AM_V_GEN)$(SHELL) $(srcdir)/mksedscript $(AM_CPPFLAGS) >$@

DISTCLEANFILES += gsftopk.1 sedscript

EXTRA_DIST = gsftopk1.sed mksedscript

