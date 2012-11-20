## Makefile.am for the TeX Live subdirectory texk/xdv2pdf/
##
## Copyright (C) 2009, 2012 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

AM_CPPFLAGS = $(KPATHSEA_INCLUDES)
AM_CFLAGS = $(WARNING_CFLAGS)
## Not yet for C++ (too many warnings)
AM_CXXFLAGS = ## $(WARNING_CXXFLAGS)
AM_OBJCFLAGS = $(WARNING_OBJCFLAGS) -fobjc-exceptions

if build
bin_PROGRAMS = T1Wrap xdv2pdf
endif build

T1Wrap_SOURCES = \
	SimplePSInterpreter.h \
	SimplePSInterpreter.m \
	T1Wrap.m

T1Wrap_LDFLAGS = -framework Cocoa
T1Wrap_LIBTOOLFLAGS = --tag CXX

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

$(xdv2pdf_OBJECTS): config.force

config.force: $(KPATHSEA_DEPEND)
	echo timestamp >config.force
	$(SHELL) ./config.status --recheck

DISTCLEANFILES = config.force

xdv2pdf_LDADD = $(KPATHSEA_LIBS)

## Rebuild libkpathsea
@KPATHSEA_RULE@

EXTRA_DIST = LICENSE.txt

