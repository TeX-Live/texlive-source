################################################################################
#
# Makefile  : TeXk / CJKutils / conversion
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <03/08/12 17:17:29 popineau>
#
################################################################################
root_srcdir = ..\..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

# This is free software; you can redistribute it and/or modify it under the
# terms of the GNU General Public License, see the file COPYING.

USE_GNUW32 = 1

!include <msvc/common.mak>

programs = \
	$(objdir)\bg5conv.exe \
	$(objdir)\cefconv.exe \
	$(objdir)\cef5conv.exe \
	$(objdir)\cefsconv.exe \
	$(objdir)\extconv.exe \
	$(objdir)\sjisconv.exe

#
# Main target
#
default: all

all: $(objdir) $(programs)

#
# Link target. setargv.obj is provided in the compiler library directory.
#
$(objdir)\bg5conv.exe: $(objdir)\bg5conv.obj $(gnuw32lib)
	$(link) $(**) $(conlibs)

$(objdir)\cefconv.exe: $(objdir)\cefconv.obj $(gnuw32lib)
	$(link) $(**) $(conlibs)

$(objdir)\cef5conv.exe: $(objdir)\cef5conv.obj $(gnuw32lib)
	$(link) $(**) $(conlibs)

$(objdir)\cefsconv.exe: $(objdir)\cefsconv.obj $(gnuw32lib)
	$(link) $(**) $(conlibs)

$(objdir)\extconv.exe: $(objdir)\extconv.obj $(gnuw32lib)
	$(link) $(**) $(conlibs)

$(objdir)\sjisconv.exe: $(objdir)\sjisconv.obj $(gnuw32lib)
	$(link) $(**) $(conlibs)

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
