# $Id: Makefile.am,v 1.1 2008/01/02 01:36:16 karl Exp $
# Makefile.am for texinfo/install-info.
# Run automake in .. to produce Makefile.in from this.
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

SUBDIRS = tests
bin_PROGRAMS = ginstall-info

# Use `ginstall-info' in the definition of PROGRAMS and in dependencies
# because automake generates an install-info target in every Makefile
# (this is easier than changing Automake.)
ginstall_info_SOURCES = install-info.c

# Transform `ginstall-info' to `install-info' before applying any
# user-specified name transformations.
transform = s/ginstall-info/install-info/; $(program_transform_name)

localedir = $(datadir)/locale
AM_CPPFLAGS = 					\
  -I$(top_srcdir)				\
  -I$(top_srcdir)/gnulib/lib			\
  -I$(top_builddir)/gnulib/lib			\
  -DLOCALEDIR=\"$(localedir)\"
LDADD = ../lib/libtxi.a $(top_builddir)/gnulib/lib/libgnu.a $(LIBINTL)
