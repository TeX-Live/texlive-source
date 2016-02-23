# Makefile for PS utilities
#
# Copyright (C) Reuben Thomas 2012
# See LICENSE for license

SUBDIRS = lib

ACLOCAL_AMFLAGS = -I m4

AM_CFLAGS = $(WERROR_CFLAGS) $(WARN_CFLAGS)
AM_CPPFLAGS = $(ISYSTEM)$(top_builddir)/lib $(ISYSTEM)$(top_srcdir)/lib -DPAPER='"$(PAPER)"'
LDADD = libpsutils.a $(top_builddir)/lib/libgnu.a

noinst_LIBRARIES = libpsutils.a
libpsutils_a_SOURCES = psutil.c psspec.c
libpsutils_a_LIBADD = $(LIBPAPER)

bin_PROGRAMS = psbook psselect pstops epsffit psnup psresize

bin_SCRIPTS = psjoin extractres

man_MANS = psutils.1 \
	psbook.1 psselect.1 pstops.1 epsffit.1 psnup.1 psresize.1 \
	psjoin.1 extractres.1 includeres.1

EXTRA_DIST = m4/gnulib-cache.m4 \
	LICENSE \
	psutil.h psspec.h \
	$(bin_SCRIPTS) \
	$(man_MANS)

all: includeres

includeres: extractres
	$(RM) $@ && $(LN_S) $(srcdir)/extractres $@

install-exec-hook:
	cd $(DESTDIR)$(bindir); $(RM) includeres; $(LN_S) extractres includeres

uninstall-hook:
	$(RM) $(DESTDIR)$(bindir)/includeres

clean-local:
	$(RM) includeres
