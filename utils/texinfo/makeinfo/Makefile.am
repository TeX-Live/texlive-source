# $Id: Makefile.am,v 1.9 2006/06/21 08:08:48 akim Exp $
# Makefile.am for texinfo/makeinfo.
# Run automake in .. to produce Makefile.in from this.
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

bin_PROGRAMS = makeinfo

localedir = $(datadir)/locale
AM_CPPFLAGS = 					\
  -I$(top_srcdir)				\
  -I$(top_srcdir)/gnulib/lib 			\
  -I$(top_builddir)/gnulib/lib			\
  -DLOCALEDIR=\"$(localedir)\"
LDADD = ../lib/libtxi.a $(top_builddir)/gnulib/lib/libgnu.a $(LIBINTL)

makeinfo_SOURCES = \
  cmds.c cmds.h defun.c defun.h \
  files.c files.h float.c float.h footnote.c footnote.h \
  html.c html.h index.c index.h insertion.c insertion.h lang.c lang.h \
  macro.c macro.h makeinfo.c makeinfo.h multi.c multi.h node.c node.h \
  sectioning.c sectioning.h toc.c toc.h xml.c xml.h xref.c xref.h

EXTRA_DIST = README

SUBDIRS = tests
