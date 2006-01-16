################################################################################
#
# Makefile  : Expat / Lib
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/09/23 17:43:55 popineau>
#
################################################################################
root_srcdir = ..\..\..
INCLUDE=$(INCLUDE);$(root_srcdir)\texk

# Makefile for expat library
# The library already compiles under win32.
# This Makefile for accomodating the needs of fpTeX.
# (maybe building a .dll later)

USE_EXPAT = 1
MAKE_EXPAT = 1

!include <msvc/common.mak>

programs = $(expatdll)
includefiles = expat.h
libfiles = $(expatlib)
objects = \
	$(objdir)\xmlrole.obj     \
	$(objdir)\xmltok.obj      \
	$(objdir)\xmlparse.obj

DEFS =  -DCOMPILED_FROM_DSP -DVERSION=\"expat_1.95.1\" $(DEFS)

default: all

all: $(objdir) $(expat)

lib: $(objdir) $(expatlib)

!ifdef EXPAT_DLL
DEFS = -DMAKE_EXPAT_DLL $(DEFS)

$(expatlib): $(objdir) $(objects)
	$(archive) /DEF $(objects)

$(expatdll): $(objects) $(objdir)\expat.res
	$(link_dll) $(**) $(expatlib:.lib=.exp) $(conlibs)

!else
$(expatlib): $(objdir) $(objects)
	$(archive) $(objects)

!endif

!include <msvc/config.mak>

!include <msvc/install.mak>

install:: install-exec install-lib install-include

!include <msvc/clean.mak>
!include <msvc/version.mak>

distclean clean::
	-@$(del) Release\* Debug\* *.aps *.ncb *.opt

!include <msvc/rdepend.mak>
!include "./depend.mak"

#
# Local Variables:
# mode: makefile
# End:

