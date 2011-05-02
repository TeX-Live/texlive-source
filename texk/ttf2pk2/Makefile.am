## Makefile.am for the TeX Live subdirectory texk/ttf2pk2/
##
## Copyright (C) 2011 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

INCLUDES = -I$(top_srcdir)/include $(KPATHSEA_INCLUDES) $(FREETYPE2_INCLUDES)
AM_CPPFLAGS = -DHAVE_LIBKPATHSEA
AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = ttf2pk ttf2tfm

common_sources = \
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
	ttfenc.c \
	ttfenc.h

ttf2pk_SOURCES = \
	$(common_sources) \
	pklib.c \
	pklib.h \
	ttf2pk.c \
	ttflib.c \
	ttflib.h

ttf2tfm_SOURCES = \
	$(common_sources) \
	case.c \
	case.h \
	tfmaux.c \
	tfmaux.h \
	ttf2tfm.c \
	ttf2tfm.h \
	ttfaux.c \
	ttfaux.h \
	vplaux.c \
	vplaux.h

$(ttf2pk_OBJECTS) $(ttf2tfm_OBJECTS): config.force

config.force: $(KPATHSEA_DEPEND) $(FREETYPE2_DEPEND)
	echo timestamp >config.force
	$(SHELL) ./config.status --recheck

DISTCLEANFILES = config.force

LDADD = $(KPATHSEA_LIBS) $(FREETYPE2_LIBS) $(ZLIB_LIBS)

## Rebuild libkpathsea
@KPATHSEA_RULE@
## Rebuild libfreetype
@FREETYPE2_RULE@

dist_man1_MANS = ttf2pk.1 ttf2tfm.1

EXTRA_DIST = data

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`

## Tests.
##
TESTS = tests/ttf2pk.test
EXTRA_DIST += $(TESTS)
## tests/ttf2pk.test
EXTRA_DIST += tests/GenR102.ttf tests/gentium-ec.enc tests/ttf2pk.cfg tests/ttfonts.map
DISTCLEANFILES += GenR-ec.vpl GenR102-ec.*

## Not used
##
EXTRA_DIST += \
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

