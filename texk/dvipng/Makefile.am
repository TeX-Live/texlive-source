## Makefile.am for the TeX Live subdirectory texk/dvipng/
##
## Copyright (C) 2009-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
#************************************************************************
#
#  Adapted for TeX Live from dvipng-1.12/Makefile.in
#  Copyright (C) 2002-2008 Jan-Åke Larsson
#
#************************************************************************
##
## We want to re-distribute the whole original dvipng source tree.
##
## With current automake (1.10.2) 'make distcheck' fails when
## DISTFILES contains a directory and files in that directory.
## Thus nodist_* for all files in $(DVIPNG_TREE).
EXTRA_DIST = $(DVIPNG_TREE)

## Patches applied to the original source tree
##
EXTRA_DIST += $(DVIPNG_TREE)-PATCHES

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

AM_CPPFLAGS = -DTEXLIVE -I$(top_srcdir)/$(DVIPNG_TREE)
AM_CPPFLAGS += $(KPATHSEA_INCLUDES) $(FREETYPE2_INCLUDES) $(GD_INCLUDES)
AM_CPPFLAGS += $(LIBPNG_INCLUDES) $(ZLIB_INCLUDES)
AM_CFLAGS = $(WARNING_CFLAGS)

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

if have_ft2
nodist_dvipng_SOURCES += \
	@DVIPNG_TREE@/enc.c \
	@DVIPNG_TREE@/fontmap.c \
	@DVIPNG_TREE@/ft.c \
	@DVIPNG_TREE@/sfd.c \
	@DVIPNG_TREE@/tfm.c
endif have_ft2

dvipng_dependencies = $(KPATHSEA_DEPEND) $(GD_DEPEND) $(FREETYPE2_DEPEND) $(LIBPNG_DEPEND)

$(dvipng_OBJECTS): config.force

config.force: $(dvipng_dependencies)
	echo timestamp >config.force
	$(SHELL) ./config.status --recheck
	$(SHELL) ./config.status Makefile config.h

DISTCLEANFILES = config.force

LDADD = $(KPATHSEA_LIBS) $(GD_LIBS) $(FREETYPE2_LIBS)
LDADD += $(LIBPNG_LIBS) $(ZLIB_LIBS)

dvigif_CPPFLAGS = -DEXEPROG=\"dvipng.exe\"
nodist_dvigif_SOURCES = callexe.c
dvigif_LDADD =

## Rebuild libkpathsea
@KPATHSEA_RULE@
## Rebuild gd
@GD_RULE@
## Rebuild freetype2
@FREETYPE2_RULE@
## Rebuild libpng
@LIBPNG_RULE@

include $(srcdir)/../../am/bin_links.am

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
dvipng.log: dvipng$(EXEEXT)

EXTRA_DIST += dvipng.test dvipng-test.dvi

CLEANFILES = dvipng-test*.gif dvipng-test*.png

