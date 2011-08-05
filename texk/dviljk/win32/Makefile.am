## Makefile.am for the TeX Live subdirectory texk/dviljk/win32/
##
## Copyright (C) 2011 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
AM_CPPFLAGS = $(KPATHSEA_INCLUDES)
AM_CFLAGS = $(WARNING_CFLAGS)

# Build WIN32 replacement for Unix shell script.
bin_PROGRAMS = dvihp

dvihp_SOURCES = \
	dvihp.c \
	mktex.h \
	mktexlib.h \
	stackenv.c \
	stackenv.h

LDADD = $(KPATHSEA_LIBS)
dvihp_DEPENDENCIES = $(KPATHSEA_DEPEND)

## Rebuild libkpathsea
@KPATHSEA_RULE@

EXTRA_DIST = ChangeLog

