## Makefile.am for the TeX Live subdirectory texk/web2c/web2c/
##
## Copyright (C) 2009-2011 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../../m4

INCLUDES = -I$(srcdir)/../..
AM_YFLAGS = -d -v
AM_CFLAGS = $(WARNING_CFLAGS)

noinst_PROGRAMS = web2c fixwrites splitup makecpool

BUILT_SOURCES = w2c/config.h

DISTCLEANFILES = w2c/config.h

# Don't include our <c-auto.h>.
w2c/config.h: ../w2c/config.h
	$(MKDIR_P) w2c
	sed -e '/c-auto\.h>/d' $(srcdir)/../w2c/config.h >$@

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
	echo timestamp >$@

stamp-fixwrites: fixwrites$(EXEEXT)
	echo timestamp >$@

stamp-splitup: splitup$(EXEEXT)
	echo timestamp >$@

stamp-makecpool: makecpool$(EXEEXT)
	echo timestamp >$@

CLEANFILES = $(nodist_noinst_DATA)

distclean-local:
	rm -rf kpathsea w2c
