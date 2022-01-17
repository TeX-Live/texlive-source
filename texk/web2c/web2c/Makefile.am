## $Id$
## Makefile.am for the TeX Live subdirectory texk/web2c/web2c/.
##
## Copyright 2015-2022 Peter Breitenlohner <tex-live@tug.org>
## Copyright 2009-2014 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.

AM_CPPFLAGS = -I$(srcdir)/../..
AM_YFLAGS = -d -v
AM_CFLAGS = $(WARNING_CFLAGS)

noinst_PROGRAMS = web2c fixwrites splitup makecpool

EXTRA_LIBRARIES = libweb2c.a

BUILT_SOURCES = w2c/config.h

DISTCLEANFILES = w2c/config.h

# Don't include our <c-auto.h>.
w2c/config.h: ../w2c/config.h
	@if test -f $@; then :; else \
	  trap 'rm -rf w2c.lock' 1 2 13 15; \
	  if mkdir w2c.lock 2>/dev/null; then \
	    $(MKDIR_P) w2c; \
	    if $(AM_V_P); then :; else echo "  GEN      $@"; fi; \
	    sed -e '/c-auto\.h>/d' $(srcdir)/../w2c/config.h >$@; \
	    rmdir w2c.lock; \
	  else \
	    while test -d w2c.lock; do sleep 1; done; \
	  fi; \
	  test -f $@; \
	fi

noinst_HEADERS = coerce.h

dist_noinst_SCRIPTS = convert

dist_noinst_DATA = \
	common.defines \
	cvtbib.sed \
	cvtmf1.sed \
	cvtmf2.sed \
	mfmp.defines \
	texmf.defines

libweb2c_a_SOURCES = \
	kps.c

$(libweb2c_a_OBJECTS): w2c/config.h
web2c_SOURCES = \
	web2c-parser.y \
	web2c-lexer.l \
	main.c \
	web2c.h
$(web2c_OBJECTS): web2c-parser.h stamp-lib
web2c_DEPENDENCIES = # don't build libweb2c.a in parallel
web2c_LDADD = libweb2c.a
#
DISTCLEANFILES += web2c-parser.output

fixwrites_SOURCES = \
	fixwrites.c
$(fixwrites_OBJECTS): stamp-lib
fixwrites_DEPENDENCIES = # don't build libweb2c.a in parallel
fixwrites_LDADD = $(web2c_LDADD)

splitup_SOURCES = \
	splitup.c
$(splitup_OBJECTS): stamp-lib
splitup_DEPENDENCIES = # don't build libweb2c.a in parallel
splitup_LDADD = $(web2c_LDADD)

makecpool_SOURCES = \
	makecpool.c

nodist_noinst_DATA = stamp-lib stamp-web2c stamp-fixwrites stamp-splitup stamp-makecpool

stamp-lib:
	@if test -f $@; then :; else \
	  trap 'rm -rf lib.lock' 1 2 13 15; \
	  if mkdir lib.lock 2>/dev/null; then \
	    $(MAKE) $(AM_MAKEFLAGS) libweb2c.a && echo timestamp >$@; \
	    rmdir lib.lock; \
	  else \
	    while test -d lib.lock; do sleep 1; done; \
	  fi; \
	  test -f $@; \
	fi

stamp-web2c: web2c$(EXEEXT)
	$(AM_V_at)echo timestamp >$@

stamp-fixwrites: fixwrites$(EXEEXT)
	$(AM_V_at)echo timestamp >$@

stamp-splitup: splitup$(EXEEXT)
	$(AM_V_at)echo timestamp >$@

stamp-makecpool: makecpool$(EXEEXT)
	$(AM_V_at)echo timestamp >$@

CLEANFILES = $(nodist_noinst_DATA)

distclean-local:
	rm -rf kpathsea w2c
