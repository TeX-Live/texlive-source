##
ACLOCAL_AMFLAGS = -I ../../../m4

INCLUDES =  -Inative -I$(srcdir)/native -I$(srcdir)/.. -I$(srcdir)/../..
AM_YFLAGS = -d -v

noinst_PROGRAMS = web2c fixwrites splitup makecpool

## Include <kpathsea/c-auto.h> instead of <web2c/c-auto.h>
native/config.h: ../config.h
	sed -e 's,web2c/c-auto,kpathsea/c-auto,' $(srcdir)/../config.h >$@

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
	native/config.h \
	web2c-parser.y \
	web2c-lexer.l \
	kps.c \
	main.c \
	web2c.h

DISTCLEANFILES = web2c-parser.output

fixwrites_SOURCES = \
	fixwrites.c \
	kps.c

splitup_SOURCES = \
	kps.c \
	splitup.c

makecpool_SOURCES = \
	makecpool.c

## Obsolete ??
##
EXTRA_DIST = convert.bat

