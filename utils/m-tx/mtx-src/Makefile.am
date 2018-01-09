## Copyright (C) 2012 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##

INCLUDES = -I$(srcdir)/libp2c
AM_CFLAGS = #$(WARNING_CFLAGS)

bin_PROGRAMS = prepmx

## prepmx_SOURCES =  config.h

prepmx_SOURCES = \
	analyze.c \
	analyze.h \
	cfuncs.c \
	cfuncs.h \
	control.c \
	control.h \
	files.c \
	files.h \
	globals.c \
	globals.h \
	lyrics.c \
	lyrics.h \
	mtx.c \
	mtx.h \
	mtxline.c \
	mtxline.h \
	multfile.c \
	multfile.h \
	notes.c \
	notes.h \
	preamble.c \
	preamble.h \
	prepmx.c \
	status.c \
	status.h \
##      strings.c \  ## p2c generated, replaced by cfuncs.c
	strings.h \
	uptext.c \
	uptext.h \
	utility.c \
	utility.h

EXTRA_DIST = \
	analyze.pas \
	control.pas \
	files.pas \
	globals.pas \
	lyrics.pas \
	mtx.pas \
	mtxline.pas \
	multfile.pas \
	notes.pas \
	preamble.pas \
	prepmx.pas \
	status.pas \
	strings.pas \
	uptext.pas \
	utility.pas  

LDADD = libp2c.a

noinst_LIBRARIES = libp2c.a

libp2c_a_SOURCES = \
	libp2c/p2c.h \
	libp2c/p2clib.c

## Tests
##
TESTS = mtx.test
EXTRA_DIST += $(TESTS)

## mtx.test
EXTRA_DIST += tests/mozart.mtx tests/mozart.pmx 

EXTRA_DIST += Makefile.orig Bugs Corrections MAINTENANCE Makefile.p2c 
EXTRA_DIST += configure.ac Makefile.am
DISTCLEANFILES = mozart*

