################################################################################
#
# Makefile  : TeXk / CJKutils / wrappers
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/01/21 11:49:32 popineau>
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

USE_KPATHSEA = 1
USE_GNUW32 = 1

!include <msvc/common.mak>

programs = $(objdir)\cjklatex.exe
linked_programs = $(objdir)\bg5latex.exe $(objdir)\bg5pltx.exe $(objdir)\cef5ltx.exe \
	$(objdir)\ceflatex.exe $(objdir)\cefsltx.exe $(objdir)\gbklatex.exe   \
	$(objdir)\sjisltx.exe

#
# Main target
#
default: all

all: $(objdir) $(programs)

$(objdir)\cjklatex.exe: $(kpathsealib) $(objdir)\cjklatex.obj
	$(link) $(objdir)\cjklatex.obj $(kpathsealib) $(conlibs)

!include <msvc/config.mak>
#
# Link target. setargv.obj is provided in the compiler library directory.
#

!include <msvc/install.mak>

install:: install-exec

install-exec::
	-@echo $(verbose) & ( \
		for %%i in ($(linked_programs)) do \
			$(copy) $(objdir)\cjklatex.exe $(bindir)\%%~nxi \
	)

!include <msvc/clean.mak>
!include <msvc/rdepend.mak>
!include "./depend.mak"

#
# Local Variables:
# mode: makefile
# End:
