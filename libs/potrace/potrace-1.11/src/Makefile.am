## Copyright (C) 2001-2013 Peter Selinger.
## This file is part of Potrace. It is free software and it is covered
## by the GNU General Public License. See the file COPYING for details.

## Process this file with automake to produce Makefile.in

AM_CFLAGS = $(CADD)

#----------------------------------------------------------------------
# programs:

bin_PROGRAMS = potrace mkbitmap
EXTRA_PROGRAMS = potracelib_demo

potrace_SOURCES = main.c main.h bitmap_io.c bitmap_io.h backend_eps.c	\
 backend_eps.h flate.c flate.h lists.h greymap.c greymap.h render.c	\
 render.h backend_pgm.c backend_pgm.h backend_svg.c backend_svg.h	\
 backend_xfig.c backend_xfig.h backend_dxf.c backend_dxf.h		\
 backend_pdf.c backend_pdf.h backend_geojson.c backend_geojson.h	\
 lzw.c lzw.h bitops.h auxiliary.h potracelib.h bitmap.h curve.h		\
 platform.h progress.h progress_bar.c progress_bar.h bbox.c bbox.h	\
 trans.c trans.h
potrace_LDADD = libpotrace.la @EXTRA_OBJS@ -lm @EXTRA_LIBS@
potrace_DEPENDENCIES = libpotrace.la @EXTRA_OBJS@

mkbitmap_SOURCES = mkbitmap.c bitmap_io.c bitmap_io.h greymap.c	\
 greymap.h platform.h
mkbitmap_LDADD = @EXTRA_OBJS@ -lm
mkbitmap_DEPENDENCIES = @EXTRA_OBJS@

potracelib_demo_SOURCES = potracelib_demo.c
potracelib_demo_LDADD = libpotrace.la -lm
potracelib_demo_DEPENDENCIES = libpotrace.la

#----------------------------------------------------------------------
# Potrace library:

libsources = curve.c curve.h trace.c trace.h decompose.c		\
 decompose.h potracelib.c potracelib.h lists.h auxiliary.h bitmap.h	\
 progress.h

lib_LTLIBRARIES = $(INSTLIBS)
EXTRA_LTLIBRARIES = libpotrace.la
libpotrace_la_SOURCES = $(libsources)
libpotrace_la_LDFLAGS = -version-info 0:1:0 -rpath '$(libdir)' \
 -export-symbols $(srcdir)/libpotrace-export.sym -no-undefined
libpotrace_la_LIBADD = -lm
MOSTLYCLEANFILES = libpotrace.la

# header files to install
include_HEADERS = $(INSTHEADERS)
EXTRA_HEADERS = potracelib.h

#----------------------------------------------------------------------
# other stuff to distribute

EXTRA_DIST = getopt.c getopt1.c getopt.h libpotrace-export.sym
