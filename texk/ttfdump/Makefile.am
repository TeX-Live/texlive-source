## Makefile.am for the TeX Live subdirectory texk/ttfdump/
##
## Copyright (C) 2009-2012 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

INCLUDES = -I$(top_srcdir)/include $(KPATHSEA_INCLUDES)
AM_CPPFLAGS = -DUNIX -DKPATHSEA
AM_CFLAGS = $(WARNING_CFLAGS)

noinst_LIBRARIES = libttf.a

libttf_a_SOURCES = \
	include/gcache.h \
	include/gpos.h \
	include/gstate.h \
	include/gsub.h \
	include/loadtable.h \
	include/os2_P.h \
	include/otftables.h \
	include/protos.h \
	include/tables.h \
	include/ttc.h \
	include/ttf.h \
	include/ttfutil.h \
	libttf/cmap.c \
	libttf/cvt.c \
	libttf/disasm.c \
	libttf/font.c \
	libttf/fpgm.c \
	libttf/gasp.c \
	libttf/gcache.c \
	libttf/glyf.c \
	libttf/gpos.c \
	libttf/gstate.c \
	libttf/gsub.c \
	libttf/hdmx.c \
	libttf/head.c \
	libttf/hhea.c \
	libttf/hmtx.c \
	libttf/kern.c \
	libttf/loca.c \
	libttf/ltsh.c \
	libttf/maxp.c \
	libttf/name.c \
	libttf/os2.c \
	libttf/otfcommon.c \
	libttf/pclt.c \
	libttf/post.c \
	libttf/prep.c \
	libttf/tabledir.c \
	libttf/ttc.c \
	libttf/ttfread.c \
	libttf/ttfutil.c \
	libttf/vdmx.c \
	libttf/vhea.c \
	libttf/vmtx.c

## Not used (interp.c would just produce compiler warnings).
##
EXTRA_DIST = \
	include/stack.h \
	libttf/fixed.c \
	libttf/interp.c \
	libttf/stack.c

$(libttf_a_OBJECTS): $(KPATHSEA_DEPEND)

bin_PROGRAMS = ttfdump

ttfdump_SOURCES = src/ttfdump.c

LDADD = libttf.a $(KPATHSEA_LIBS)

dist_man1_MANS = docs/ttfdump.1

## Not used
##
EXTRA_DIST += \
	docs/ttfdump.dvi \
	docs/ttfdump.html \
	docs/ttfdump.ps

## Rebuild libkpathsea
@KPATHSEA_RULE@

## Tests.
##
TESTS = tests/ttfdump.test
EXTRA_DIST += $(TESTS)
DISTCLEANFILES =
## tests/ttfdump.test
EXTRA_DIST += \
	tests/FonetikaDaniaIwonaeRegular.ttf \
	tests/texmfhome.otf \
	tests/ttfdump.glyf \
	tests/ttfdump.gpos \
	tests/ttfdump.gsub \
	tests/ttfdump.head \
	tests/ttfdump.os_2
DISTCLEANFILES += \
	FonetikaDaniaIwonaeRegular.ttf \
	texmfhome.otf \
	ttftest.*

## Not used
##
EXTRA_DIST += ILX NOTE README.bg5

