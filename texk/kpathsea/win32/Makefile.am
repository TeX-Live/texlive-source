## Makefile.am for the TeX Live subdirectory texk/kpathsea/win32/
##
## Copyright (C) 2011-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
AM_CPPFLAGS = -I$(top_builddir)/.. -I$(top_srcdir)/..
AM_CPPFLAGS += -DWEB2CVERSION='"$(WEB2CVERSION)"'
AM_CFLAGS = $(WARNING_CFLAGS)

# Build WIN32 replacements for Unix shell scripts.
bin_PROGRAMS = \
	mktexfmt \
	mktexlsr \
	mktexmf \
	mktexpk \
	mktextfm \
	mktexupd

mktexfmt_SOURCES = \
	mktexfmt.c

mktexlsr_SOURCES = \
	dirutil.c \
	mkpaths.c \
	mktex.h \
	mktexlsr.c

mktexmf_SOURCES = \
	dirutil.c \
	getdestdir.c \
	mkpaths.c \
	mktex.h \
	mktexmf.c \
	mktexupd.c

mktexpk_SOURCES = \
	dirutil.c \
	getdestdir.c \
	mkpaths.c \
	mktex.h \
	mktexpk.c \
	mktexupd.c

mktextfm_SOURCES = \
	dirutil.c \
	getdestdir.c \
	mkpaths.c \
	mktex.h \
	mktextfm.c \
	mktexupd.c

mktexupd_SOURCES = \
	mkpaths.c \
	mktex.h \
	mktexupdmain.c \
	mktexupd.c

LDADD = ../libkpathsea.la

install-exec-hook:
	cp -p $(DESTDIR)$(bindir)/mktexlsr$(EXEEXT) $(DESTDIR)$(bindir)/texhash$(EXEEXT)

uninstall-hook:
	rm -f $(DESTDIR)$(bindir)/texhash$(EXEEXT)

## Rebuild libkpathsea
../libkpathsea.la: ${top_srcdir}/../kpathsea/*.[ch] ../paths.h
	cd .. && $(MAKE) $(AM_MAKEFLAGS) rebuild
../paths.h:
	cd .. && $(MAKE) $(AM_MAKEFLAGS) rebuild

EXTRA_DIST = ChangeLog

