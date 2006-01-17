################################################################################
#
# Makefile  : TeXInfo/libtxi
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/01/20 18:19:01 popineau>
#
################################################################################
root_srcdir = ..\..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

# Copyright (C) 1994, 1995, 1996, 1997 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy, distribute and modify it.

USE_GNUW32 = 1

!include <msvc/common.mak>

DEFS = -I.. -DHAVE_CONFIG_H $(DEFS)
objects =  $(objdir)\substring.obj $(objdir)\xexit.obj

default: all

all: $(objdir) $(objdir)\libtxi.lib

$(objdir)\libtxi.lib: $(objdir) $(objects)
	$(archive) $(objects)

..\config.h: ..\config.in
	pushd .. & $(make) config.h & popd

!include <msvc/install.mak>

!include <msvc/clean.mak>
!include <msvc/rdepend.mak>
!include "./depend.mak"

#
# Local Variables:
# mode: makefile
# End: