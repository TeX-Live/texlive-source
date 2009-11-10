## Makefile.am for the TeX Live subdirectory texk/ttf2pk/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

INCLUDES = -I$(top_srcdir)/include $(KPATHSEA_INCLUDES) $(FREETYPE_INCLUDES)
AM_CPPFLAGS = -DHAVE_LIBKPATHSEA
AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = ttf2pk ttf2tfm

ttf2pk_SOURCES = \
	errormsg.c \
	errormsg.h \
	filesrch.c \
	filesrch.h \
	ligkern.c \
	ligkern.h \
	newobj.c \
	newobj.h \
	parse.c \
	parse.h \
	pklib.c \
	pklib.h \
	subfont.c \
	subfont.h \
	texenc.c \
	texenc.h \
	ttf2pk.c \
	ttfenc.c \
	ttfenc.h \
	ttflib.c \
	ttflib.h

ttf2tfm_SOURCES = \
	case.c \
	case.h \
	errormsg.c \
	errormsg.h \
	filesrch.c \
	filesrch.h \
	ligkern.c \
	ligkern.h \
	newobj.c \
	newobj.h \
	parse.c \
	parse.h \
	subfont.c \
	subfont.h \
	texenc.c \
	texenc.h \
	tfmaux.c \
	tfmaux.h \
	ttf2tfm.c \
	ttf2tfm.h \
	ttfaux.c \
	ttfaux.h \
	ttfenc.c \
	ttfenc.h \
	vplaux.c \
	vplaux.h

$(ttf2pk_OBJECTS) $(ttf2tfm_OBJECTS): config.force

config.force: $(KPATHSEA_DEPEND) $(FREETYPE_DEPEND)
	echo timestamp >config.force
	$(SHELL) ./config.status --recheck

DISTCLEANFILES = config.force

LDADD = $(KPATHSEA_LIBS) $(FREETYPE_LIBS)

## Rebuild libkpathsea
@KPATHSEA_RULE@
## Rebuild libttf
@FREETYPE_RULE@

dist_man1_MANS = ttf2pk.1 ttf2tfm.1

## Not used
##
EXTRA_DIST = \
	data \
	BUGS \
	MakeSub.in \
	Makefile.dm \
	dvidrv.btm \
	dvidrv.doc \
	emdir.c \
	emdir.h \
	emtexdir.c \
	emtexdir.h \
	ttf2pk.doc

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`

