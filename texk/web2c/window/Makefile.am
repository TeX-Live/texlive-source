##
INCLUDES = -I$(top_srcdir) $(KPATHSEA_INCLUDES)

AM_CPPFLAGS = $(X_CFLAGS)

## Rebuild libkpathsea
@KPATHSEA_RULE@

## Built on demand, after `../mfd.h' has been created
EXTRA_LIBRARIES = libwindow.a

libwindow_a_SOURCES = \
	amiga.c \
	epsf.c \
	hp2627.c \
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

