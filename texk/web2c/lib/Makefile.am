## Makefile.am for the TeX Live subdirectory texk/web2c/lib/
##
## Copyright (C) 2009-2012 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
AM_CPPFLAGS = -I$(top_srcdir) $(KPATHSEA_INCLUDES)
AM_CFLAGS = $(WARNING_CFLAGS)

## Rebuild libkpathsea
@KPATHSEA_RULE@

noinst_LIBRARIES = lib.a
if EUPTEX
noinst_LIBRARIES += libp.a
endif EUPTEX
EXTRA_LIBRARIES = libp.a

## We don't compile `texmfmp.c'; this file is included from 'texextra.c'
## for TeX, 'mfextra.c' for Metafont, ...
lib_a_SOURCES = \
	basechsuffix.c \
	chartostring.c \
	coredump.c \
	eofeoln.c \
	fprintreal.c \
	inputint.c \
	input2int.c \
	lib.h \
	main.c \
	openclose.c \
	printversion.c \
	setupvar.c \
	uexit.c \
	usage.c \
	version.c \
	zround.c

## Replacements for pTeX.
##
libp_a_CPPFLAGS = -DPTEX $(PTEXENC_INCLUDES) $(AM_CPPFLAGS)
libp_a_SOURCES = \
	openclose.c \
	printversion.c

## Not yet used
##
EXTRA_DIST = alloca.c

