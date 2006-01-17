################################################################################
#
# Makefile  : TeXInfo / Utils
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/02/01 11:16:42 popineau>
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

USE_ZLIB=1
USE_GNUW32=1

!include <msvc/common.mak>

DEFS = -I.. -DHAVE_CONFIG_H -DKPATHSEA=1 $(DEFS) -I../lib

libtxi = ..\lib\$(objdir)\libtxi.lib

install_info_objects=$(objdir)\install-info.obj
texindex_objects = $(objdir)\texindex.obj

programs = $(objdir)\install-info.exe $(objdir)\texindex.exe

default all: $(objdir) $(programs)

$(objdir)\install-info.exe: $(install_info_objects) $(libtxi) $(gnuw32lib)
	$(link) $(**) $(conlibs)

$(objdir)\texindex.exe: $(texindex_objects) $(libtxi) $(gnuw32lib)
	$(link) $(**) $(setargvobj) $(conlibs)

$(libtxi):  ../lib/*.c ../lib/*.h
	pushd ..\lib & $(make) all & popd

..\config.h: ..\config.in
	pushd .. & $(make) configure & popd

!include <msvc/config.mak>
!include <msvc/install.mak>

install:: install-exec

!include <msvc/clean.mak>
!include <msvc/rdepend.mak>
!include "./depend.mak"

#
# Local Variables:
# mode: makefile
# End: