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
AM_CFLAGS = $(WARNING_CFLAGS)

DVIPS = dvips -Ppdf

TEXI2HTML = texi2html

# In order that `make distcheck' succeeds, we must not attempt to rebuild
# dvipng.info if dvipng.help has not changed,  Thus we delegate rebuilding
# dvipng.help and  dvipng.info, if necessary, to subdirectories. 
SUBDIRS = . help doc

## #################################### The program
##
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

if have_gif
install-exec-hook:
	cd $(DESTDIR)$(bindir) && \
	  rm -f dvigif$(EXEEXT) && \
	  $(LN_S) dvipng$(EXEEXT) dvigif$(EXEEXT)
uninstall-hook:
	rm -f $(DESTDIR)$(bindir)/dvigif$(EXEEXT)
endif have_gif

## #################################### The test
##
test: test_dvipng.dvi dvipng$(EXEEXT)
	./dvipng -T tight -strict test_dvipng
	echo View the result e.g. with xv test_dvipng\*.png

test_dvipng.dvi: test_dvipng.tex
	latex $(srcdir)/test_dvipng.tex

TESTS = dvipng.test

EXTRA_DIST = dvipng.test test_dvipng.tex

CLEANFILES = missfont.log test_dvipng.aux test_dvipng.dvi test_dvipng.log test_dvipng*.png

## #################################### Maintainer targets
##
## Make sure INSTALL and README exist
##
EXTRA_DIST += ChangeLog.0 INSTALL README RELEASE

## Not used
##
EXTRA_DIST += miktex.h miktex.mak

## #################################### Build as part of the TeX Live tree
##
EXTRA_DIST += ChangeLog.TL

## Patches applied to the dvipng-1.12 distribution
##
EXTRA_DIST += dvipng-1.12-patches

## Original files from the dvipng distribution
##
EXTRA_DIST += Makefile.in.orig aclocal.m4.orig configure.ac.orig

