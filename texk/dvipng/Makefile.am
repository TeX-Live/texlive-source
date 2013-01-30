## Makefile.am for the TeX Live subdirectory texk/dvipng/
##
## Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
#************************************************************************
#
#  Adapted for TeX Live from dvipng-1.12/Makefile.in
#  Copyright (C) 2002-2008 Jan-Åke Larsson
#
#************************************************************************
##
ACLOCAL_AMFLAGS = -I ../../m4 -I m4

## We want to re-distribute the whole original dvipng source tree.
##
## With current automake (1.10.2) 'make distcheck' fails when
## DISTFILES contains a directory and files in that directory.
## Thus nodist_* for all files in $(DVIPNG_TREE).
EXTRA_DIST = $(DVIPNG_TREE)

## Patches applied to the original source tree
##
EXTRA_DIST += $(DVIPNG_TREE)-PATCHES

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn -o -name .deps -o -name .dirstamp -o -name '*.$(OBJEXT)'`

AM_CPPFLAGS = -I$(top_srcdir)/$(DVIPNG_TREE)
AM_CPPFLAGS += $(KPATHSEA_INCLUDES) $(FREETYPE2_INCLUDES) $(GD_INCLUDES)
AM_CPPFLAGS +=  $(T1LIB_INCLUDES) $(LIBPNG_INCLUDES) $(ZLIB_INCLUDES)
AM_CFLAGS = $(WARNING_CFLAGS)

DVIPS = dvips -Ppdf

TEXI2HTML = texi2html

# In order that `make distcheck' succeeds, we must not attempt to rebuild
# dvipng.info if dvipng.help has not changed,  Thus we delegate rebuilding
# dvipng.help and dvipng.info, if necessary, to subdirectories.
SUBDIRS = . help doc

bin_PROGRAMS = dvipng

nodist_dvipng_SOURCES = \
	@DVIPNG_TREE@/color.c \
	@DVIPNG_TREE@/draw.c \
	@DVIPNG_TREE@/dvi.c \
	@DVIPNG_TREE@/dvipng.c \
	@DVIPNG_TREE@/font.c \
	@DVIPNG_TREE@/misc.c \
	@DVIPNG_TREE@/papersiz.c \
	@DVIPNG_TREE@/pk.c \
	@DVIPNG_TREE@/ppagelist.c \
	@DVIPNG_TREE@/set.c \
	@DVIPNG_TREE@/special.c \
	@DVIPNG_TREE@/vf.c

if have_t1
nodist_dvipng_SOURCES += \
	@DVIPNG_TREE@/t1.c
endif have_t1

if have_ft2
nodist_dvipng_SOURCES += \
	@DVIPNG_TREE@/ft.c \
	@DVIPNG_TREE@/sfd.c
endif have_ft2

if have_ft2_or_t1
nodist_dvipng_SOURCES += \
	@DVIPNG_TREE@/enc.c \
	@DVIPNG_TREE@/fontmap.c \
	@DVIPNG_TREE@/tfm.c
endif have_ft2_or_t1

dvipng_dependencies = $(KPATHSEA_DEPEND) $(T1LIB_DEPEND) $(GD_DEPEND)

$(dvipng_OBJECTS): config.force

config.force: $(dvipng_dependencies)
	echo timestamp >config.force
	$(SHELL) ./config.status --recheck
	$(SHELL) ./config.status Makefile config.h

DISTCLEANFILES = config.force

LDADD = $(KPATHSEA_LIBS) $(GD_LIBS) $(FREETYPE2_LIBS)
LDADD += $(T1LIB_LIBS) $(LIBPNG_LIBS) $(ZLIB_LIBS)

dvigif_CPPFLAGS = -DEXEPROG=\"dvipng.exe\"
nodist_dvigif_SOURCES = callexe.c
dvigif_LDADD =

## Rebuild libkpathsea
@KPATHSEA_RULE@
## Rebuild libgd
@GD_RULE@
## Rebuild libt1
@T1LIB_RULE@

include $(srcdir)/../am/bin_links.am

bin_links = dvipng$(EXEEXT):dvigif

if have_gif
if WIN32
bin_PROGRAMS += dvigif
else !WIN32
install-exec-hook: install-bin-links
uninstall-hook: uninstall-bin-links
endif !WIN32
endif have_gif

TESTS = dvipng.test

EXTRA_DIST += dvipng.test dvipng-test.dvi

CLEANFILES = dvipng-test*.png

