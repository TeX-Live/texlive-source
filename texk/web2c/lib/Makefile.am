## Makefile.am for the TeX Live subdirectory texk/web2c/lib/
##
## Copyright (C) 2009, 2010 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
INCLUDES = -I$(top_builddir)/.. -I$(top_srcdir) $(KPATHSEA_INCLUDES)
AM_CFLAGS = $(WARNING_CFLAGS)

## Rebuild libkpathsea
@KPATHSEA_RULE@

noinst_LIBRARIES = lib.a

## We don't compile `texmfmp.c'; this file is converted into 'texextra.c'
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

## Not yet used
##
EXTRA_DIST = alloca.c win32main.c

