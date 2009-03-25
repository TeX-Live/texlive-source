##
#************************************************************************
#
#  Part of the dvipng distribution
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as
#  published by the Free Software Foundation, either version 3 of the
#  License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful, but
#  WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this program. If not, see
#  <http://www.gnu.org/licenses/>.
#
#  Copyright (C) 2002-2008 Jan-Åke Larsson
#
#************************************************************************
#
#  Adapted for TeX Live by  Peter Breitenlohner  <peb@mppmu.mpg.de>
#  All settings here are our fault, don't blame the dvipng maintainer for it.
#
#************************************************************************
##
ACLOCAL_AMFLAGS = -I ../../m4

INCLUDES = $(KPATHSEA_INCLUDES) $(FREETYPE2_INCLUDES) $(GD_INCLUDES)
INCLUDES +=  $(T1LIB_INCLUDES) $(LIBPNG_INCLUDES) $(ZLIB_INCLUDES)
AM_CPPFLAGS =

bin_PROGRAMS = dvipng

dvipng_SOURCES = \
	color.c \
	commands.h \
	draw.c \
	dvi.c \
	dvipng.c \
	dvipng.h \
	font.c \
	misc.c \
	papersiz.c \
	pk.c \
	ppagelist.c \
	set.c \
	special.c \
	vf.c

dvipng_DEPENDENCIES = $(KPATHSEA_DEPEND) $(FREETYPE2_DEPEND) $(GD_DEPEND)
dvipng_DEPENDENCIES += $(T1LIB_DEPEND) $(LIBPNG_DEPEND) $(ZLIB_DEPEND)

LDADD = $(KPATHSEA_LIBS) $(FREETYPE2_LIBS) $(GD_LIBS)
LDADD += $(T1LIB_LIBS) $(LIBPNG_LIBS) $(ZLIB_LIBS)

## Rebuild libkpathsea
@KPATHSEA_RULE@
## Rebuild libfreetype
@FREETYPE2_RULE@
## Rebuild libgd
@GD_RULE@
## Rebuild libt1
@T1LIB_RULE@
## Rebuild libpng
@LIBPNG_RULE@
## Rebuild libz
@ZLIB_RULE@

dist_man1_MANS = dvipng.1

## Not yet used
##
EXTRA_DIST = \
	ChangeLog.0 \
	ChangeLog.TL \
	RELEASE \
	dvipng.dvi \
	dvipng.help \
	dvipng.info \
	dvipng.texi \
	enc.c \
	fontmap.c \
	ft.c \
	install.texi \
	macros.texi \
	miktex.h \
	miktex.mak \
	readme.texi \
	sfd.c \
	t1.c \
	test_dvipng.tex \
	tfm.c

EXTRA_DIST += \
	dvipng.dvi.orig \
	dvipng.help.orig \
	dvipng.info.orig

## Original files from dvipng-$(DVIPNG_VERSION)
##
EXTRA_DIST += \
	Makefile.in-$(DVIPNG_VERSION) \
	aclocal.m4-$(DVIPNG_VERSION) \
	config.h.in-$(DVIPNG_VERSION) \
	configure-$(DVIPNG_VERSION) \
	configure.ac-$(DVIPNG_VERSION) \
	special.c-$(DVIPNG_VERSION)

## Eventually use these macro files
##
EXTRA_DIST += m4/gs-device.m4 m4/makeinfo.m4

## Eventually delete these files
##
EXTRA_DIST += Makefile.in.orig acinclude.m4.orig configure.ac.orig

EXTRA_DIST += Makefile.am.demo Makefile.in.work configure.ac.demo configure.ac.work

