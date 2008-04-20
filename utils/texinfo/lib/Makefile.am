# $Id: Makefile.am,v 1.11 2007/07/01 18:07:36 karl Exp $
# Makefile.am for texinfo/lib.
# Run automake in .. to produce Makefile.in from this.
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

noinst_LIBRARIES = libtxi.a

AM_CPPFLAGS = -I$(top_srcdir)			\
  -I$(top_srcdir)/gnulib/lib			\
  -I$(top_builddir)/gnulib/lib			\
  -DLOCALEDIR=\"$(localedir)\"

libtxi_a_SOURCES = substring.c xexit.c

EXTRA_DIST = README
