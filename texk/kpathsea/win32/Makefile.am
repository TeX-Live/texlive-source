## Makefile.am for the TeX Live subdirectory texk/dviljk/win32/
##
## Copyright (C) 2011 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
AM_CPPFLAGS = -I$(top_builddir)/.. -I$(top_srcdir)/..
AM_CFLAGS = $(WARNING_CFLAGS)

# Build WIN32 replacements for Unix shell scripts.
bin_PROGRAMS = mktexmf mktexpk mktextfm mktexupd

mktexmf_SOURCES = \
	getdestdir.c \
	getdestdir.h \
	mktexmf.c \
	mktexupd.c \
	mktexupd.h

mktexpk_SOURCES = \
	getdestdir.c \
	getdestdir.h \
	mktexpk.c \
	mktexupd.c \
	mktexupd.h

mktextfm_SOURCES = \
	getdestdir.c \
	getdestdir.h \
	mktextfm.c \
	mktexupd.c \
	mktexupd.h

mktexupd_SOURCES = \
	mktexupdmain.c \
	mktexupd.c \
	mktexupd.h

LDADD = ../libkpathsea.la

## Rebuild libkpathsea
../libkpathsea.la: ${top_srcdir}/../kpathsea/*.[ch] ../paths.h
	cd .. && $(MAKE) $(AM_MAKEFLAGS) rebuild
../paths.h:
	cd .. && $(MAKE) $(AM_MAKEFLAGS) rebuild

EXTRA_DIST = ChangeLog

