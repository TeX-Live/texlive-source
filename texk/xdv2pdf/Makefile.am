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

## Eventually delete these files
##
EXTRA_DIST += \
	Makefile.in.orig \
	acconfig.h.dist \
	acconfig.h.in \
	configure.in.orig

