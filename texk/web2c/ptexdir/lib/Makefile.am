## Makefile.am for the TeX Live subdirectory texk/web2c/ptexdir/lib/
##
## Copyright (C) 2010 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
INCLUDES = -I$(top_builddir)/.. -I$(top_srcdir) $(KPATHSEA_INCLUDES)
AM_CFLAGS = $(WARNING_CFLAGS)

## Rebuild libkpathsea
@KPATHSEA_RULE@

noinst_LIBRARIES = lib.a

# Replacement for files from ../../lib/
lib_a_SOURCES = \
	openclose.c \
	printversion.c
