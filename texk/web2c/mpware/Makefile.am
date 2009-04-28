## Makefile.am for the TeX Live subdirectory texk/web2c/mpware/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
INCLUDES = -I$(top_builddir)/.. -I$(top_srcdir) $(KPATHSEA_INCLUDES)

proglib = ../lib/lib.a

## Automake automatically uses $(proglib)=../lib/lib.a as dependency.
## This does not work for the configure substitution  $(KPATHSEA_LIBS),
## thus we let $(proglib) artificially depend on $(KPATHSEA_LIBS).

## Rebuild $(proglib)
$(proglib): $(KPATHSEA_DEPEND) ${top_srcdir}/lib/*.c
	cd ../lib && $(MAKE) $(AM_MAKEFLAGS)

## Rebuild libkpathsea
@KPATHSEA_RULE@

bin_PROGRAMS = dmp makempx mpto newer

dmp_SOURCES = dmp.c dmp.h dmp_ffile.c
dmp_LDADD = $(proglib) $(KPATHSEA_LIBS)

makempx_SOURCES = makempx.c
makempx_LDADD = $(proglib) $(KPATHSEA_LIBS)

mpto_SOURCES = mpto.c

newer_SOURCES = newer.c

## Obsolete ?
##
EXTRA_DIST = makempx.in

## Evetually delete these files
##
EXTRA_DIST += Makefile.in.orig

