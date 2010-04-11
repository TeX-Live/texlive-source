## Makefile.am for the TeX Live subdirectory texk/web2c/ptexdir/lib/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
INCLUDES = -I$(top_builddir)/.. -I$(top_srcdir) $(KPATHSEA_INCLUDES)
AM_CFLAGS = $(WARNING_CFLAGS)

## Rebuild libkpathsea
@KPATHSEA_RULE@

noinst_LIBRARIES = lib.a

## We don't compile `texmfmp.c'; this file is converted into 'texextra.c'
## for TeX, 'mfextra.c' for Metafont, 'mfextra.c' for MetaPost, ...
lib_a_SOURCES = \
	openclose.c \
	printversion.c \
	usage.c
