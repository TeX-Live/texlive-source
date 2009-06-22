## Makefile.am for the TeX Live subdirectory texk/dtl/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

INCLUDES = $(KPATHSEA_INCLUDES)
AM_CPPFLAGS = -DUNIX -DKPATHSEA
## We don't use libpathsea, so don't overload fopen and stuff
AM_CPPFLAGS += -DNO_DEBUG
AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = dt2dv dv2dt

dt2dv_SOURCES = dt2dv.c dtl.h

dist_man1_MANS = dt2dv.man dv2dt.man

dist_check_SCRIPTS = dtl.test

TESTS = dtl.test

EXTRA_DIST = edited.txt

CLEANFILES = testfile.*

## Not used
##
EXTRA_DIST += \
	dtl.doc \
	dvi.doc \
	example.tex \
	hello.tex \
	man2ps \
	tripvdu.tex

