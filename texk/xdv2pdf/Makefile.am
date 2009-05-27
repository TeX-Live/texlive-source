## Makefile.am for the TeX Live subdirectory texk/dtl/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

INCLUDES = $(KPATHSEA_INCLUDES)
AM_CPPFLAGS =

if build
bin_PROGRAMS = T1Wrap xdv2pdf
endif build

T1Wrap_SOURCES = \
	SimplePSInterpreter.h \
	SimplePSInterpreter.m \
	T1Wrap.m

T1Wrap_LDFLAGS = -framework Cocoa

xdv2pdf_SOURCES = \
	DVIops.h \
	xdv2pdf.cpp \
	xdv2pdf_globals.h \
	xdv2pdf_main.c \
	xdv_kpse_formats.h \
	xdv2pdf_specials.cpp

## Included by xdv2pdf.cpp
EXTRA_xdv2pdf_SOURCES = appleGlyphNames.c

xdv2pdf_LDFLAGS = -framework Carbon -framework QuickTime

xdv2pdf_LDADD = $(KPATHSEA_LIBS)

## Rebuild libkpathsea
@KPATHSEA_RULE@

EXTRA_DIST = LICENSE.txt

