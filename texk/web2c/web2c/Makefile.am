## Makefile.am for the TeX Live subdirectory texk/web2c/web2c/
##
## Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../../m4

AM_CPPFLAGS = -I$(srcdir)/../..
AM_YFLAGS = -d -v
AM_CFLAGS = $(WARNING_CFLAGS)

noinst_PROGRAMS = web2c fixwrites splitup makecpool

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

web2c_SOURCES = \
	web2c-parser.y \
	web2c-lexer.l \
	kps.c \
	main.c \
	web2c.h

$(web2c_OBJECTS): web2c-parser.h w2c/config.h

DISTCLEANFILES += web2c-parser.output

fixwrites_SOURCES = \
	fixwrites.c \
	kps.c

$(fixwrites_OBJECTS): w2c/config.h

splitup_SOURCES = \
	kps.c \
	splitup.c

$(splitup_OBJECTS): w2c/config.h

makecpool_SOURCES = \
	makecpool.c

nodist_noinst_DATA = stamp-web2c stamp-fixwrites stamp-splitup stamp-makecpool

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
