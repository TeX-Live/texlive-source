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
ACLOCAL_AMFLAGS = -I ../../m4 -I m4

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

if have_t1
dvipng_SOURCES += t1.c
endif have_t1

if have_ft2
dvipng_SOURCES += ft.c sfd.c
endif have_ft2

if have_ft2_or_t1
dvipng_SOURCES += enc.c fontmap.c tfm.c
endif have_ft2_or_t1

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

info_TEXINFOS = dvipng.texi
dvipng_TEXINFOS = readme.texi install.texi macros.texi dvipng.help

dist_man1_MANS = dvipng.1
dvipng.1: dvipng.texi readme.texi
	$(srcdir)/texi2pod.pl -D man $(srcdir)/dvipng.texi | \
	sed -es/@//g -es/previewlatex/preview-latex/g -es/{}//g > dvipng.pod
	pod2man --center="User commands" --release="$(PACKAGE_STRING)" \
		dvipng.pod > dvipng.1
	rm dvipng.pod

## Make sure INSTALL and README exist
##
EXTRA_DIST = ChangeLog.0 ChangeLog.TL INSTALL README RELEASE

INSTALL: install.texi
	-$(MAKEINFO) $(AM_MAKEINFOFLAGS) $(MAKEINFOFLAGS) -I $(srcdir) \
	  -D rawfile --no-headers --no-validate --no-number-sections \
	  -o $@ $<

README: readme.texi
	-$(MAKEINFO) $(AM_MAKEINFOFLAGS) $(MAKEINFOFLAGS) -I $(srcdir) \
	  -D rawfile --no-headers --no-validate --no-number-sections \
	  -o $@ $<

dist_noinst_SCRIPTS = texi2pod.pl

if have_gif

install-exec-hook:
	cd $(DESTDIR)$(bindir) && \
	  rm -f dvigif$(EXEEXT) && \
	  $(LN_S) dvipng$(EXEEXT) dvigif$(EXEEXT)

install-data-hook:
	cd $(DESTDIR)$(man1dir) && \
	  rm -f dvigif.1 && \
	  $(LN_S) dvipng.1 dvigif.1

uninstall-hook:
	rm -f $(DESTDIR)$(bindir)/dvigif$(EXEEXT)
	rm -f $(DESTDIR)$(man1dir)/dvigif.1

endif have_gif

## Not yet used
##
EXTRA_DIST += test_dvipng.tex

## Not used
##
EXTRA_DIST += miktex.h miktex.mak

## Patches applied to dvipng-$(DVIPNG_VERSION) distribution
##
EXTRA_DIST += dvipng-$(DVIPNG_VERSION)-patches

## Original files from dvipng-$(DVIPNG_VERSION)
##
EXTRA_DIST += \
	Makefile.in-$(DVIPNG_VERSION) \
	aclocal.m4-$(DVIPNG_VERSION) \
	configure.ac-$(DVIPNG_VERSION)

## Eventually use these macro files
##
EXTRA_DIST += m4/gs-device.m4

## Eventually delete these files
##
EXTRA_DIST += Makefile.in.orig acinclude.m4.orig configure.ac.orig

EXTRA_DIST += Makefile.am.demo Makefile.in.work configure.ac.demo configure.ac.work

