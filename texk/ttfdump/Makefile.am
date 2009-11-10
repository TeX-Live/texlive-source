## Makefile.am for the TeX Live subdirectory texk/ttfdump/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

## We want to distribute the whole subdirectories.
##
## With current automake (1.10.2) 'make distcheck' fails when
## DISTFILES contains a directory and files in that directory.
EXTRA_DIST = docs include libttf src

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`
	rm -f $(distdir)/libttf/Makefile.in
	rm -f $(distdir)/src/Makefile.in

INCLUDES = -I$(top_srcdir)/include
AM_CPPFLAGS = -Dlint
AM_CFLAGS = $(WARNING_CFLAGS)

noinst_LIBRARIES = libttf.a

# libttf/interp.c is not used (would just produce compiler warnings).
#
nodist_libttf_a_SOURCES = \
	libttf/cmap.c \
	libttf/cvt.c \
	libttf/disasm.c \
	libttf/font.c \
	libttf/fpgm.c \
	libttf/gasp.c \
	libttf/gcache.c \
	libttf/glyf.c \
	libttf/gstate.c \
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
	libttf/pclt.c \
	libttf/post.c \
	libttf/prep.c \
	libttf/stack.c \
	libttf/tabledir.c \
	libttf/ttc.c \
	libttf/ttfread.c \
	libttf/ttfutil.c \
	libttf/vdmx.c \
	libttf/vhea.c \
	libttf/vmtx.c

bin_PROGRAMS = ttfdump

nodist_ttfdump_SOURCES = src/ttfdump.c

LDADD = libttf.a

nodist_man1_MANS = docs/ttfdump.1

## Not used
##
EXTRA_DIST += ILX NOTE README.bg5

