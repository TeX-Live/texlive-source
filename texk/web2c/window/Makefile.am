## Makefile.am for the TeX Live subdirectory texk/web2c/window/
##
## Copyright (C) 2009, 2010 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
INCLUDES = -I$(top_srcdir) $(KPATHSEA_INCLUDES)

AM_CPPFLAGS = $(X_CFLAGS)
AM_CFLAGS = $(WARNING_CFLAGS)

## Rebuild libkpathsea
@KPATHSEA_RULE@

## Built on demand, after `../mfd.h' has been created
EXTRA_LIBRARIES = libwindow.a

libwindow_a_SOURCES = \
	amiga.c \
	epsf.c \
	hp2627.c \
	mfdisplay.h \
	mftalk.c \
	next.c \
	regis.c \
	suntools.c \
	tek.c \
	trap.c \
	uniterm.c \
	win32.c

if MFXT
libwindow_a_SOURCES += x11-Xt.c
else !MFXT
libwindow_a_SOURCES += x11-Xlib.c
endif !MFXT

DISTCLEANFILES = libwindow.a

