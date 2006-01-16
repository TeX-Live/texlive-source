################################################################################
#
# Makefile  : libttf, MSVC
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Time-stamp: <02/09/23 17:40:42 popineau>
#
################################################################################
root_srcdir = ..\..
INCLUDE=$(INCLUDE);$(root_srcdir)\texk

# Makefile for freetype library
# The library already compiles under win32.
# This Makefile for accomodating the needs of fpTeX.
# (maybe building a .dll later)

USE_GNUW32 = 1
USE_TTF = 1
MAKE_TTF = 1

!include <msvc/common.mak>

programs = $(ttfdll)
libfiles = $(ttflib)
includefiles = freetype.h fterrid.h ftnameid.h
objects = \
$(objdir)\ttapi.obj \
$(objdir)\ttcache.obj \
$(objdir)\ttcalc.obj \
$(objdir)\ttcmap.obj \
$(objdir)\ttdebug.obj \
$(objdir)\ttextend.obj \
$(objdir)\ttfile.obj \
$(objdir)\ttgload.obj \
$(objdir)\ttinterp.obj \
$(objdir)\ttload.obj \
$(objdir)\ttmemory.obj \
$(objdir)\ttmutex.obj \
$(objdir)\ttobjs.obj \
$(objdir)\ttraster.obj \
$(objdir)\ftxkern.obj \
$(objdir)\ftxpost.obj \
$(objdir)\ftxerr18.obj \
$(objdir)\ftxcmap.obj \
$(objdir)\ftxwidth.obj \
$(objdir)\ftxgsub.obj \
$(objdir)\ftxgdef.obj \
$(objdir)\ftxgpos.obj \
$(objdir)\ftxopen.obj \
$(objdir)\ftxgasp.obj


default: all

all: $(objdir) $(ttf)

lib: $(objdir) $(ttflib)

!ifdef TTF_DLL
DEFS = -DMAKE_TTF_DLL $(DEFS)

$(ttflib): $(objdir) $(objects)
	$(archive) /DEF $(objects)

$(ttfdll): $(objects) $(objdir)\libttf.res $(gnuw32lib)
	$(link_dll)  $(**) $(ttflib:.lib=.exp) $(conlibs)
!else
$(ttflib): $(objdir) $(objects)
	$(archive) $(objects)
!endif

ft_conf.h: arch\win32\ft_conf.h
	copy $** $@

$(objdir)\ftxkern.obj:  extend\ftxkern.c
	$(compile) $**
$(objdir)\ftxwidth.obj:  extend\ftxwidth.c
	$(compile) $**
$(objdir)\ftxpost.obj:  extend\ftxpost.c
	$(compile) $**
$(objdir)\ftxerr18.obj: extend\ftxerr18.c
	$(compile) $**
$(objdir)\ftxcmap.obj:  extend\ftxcmap.c
	$(compile) $**
$(objdir)\freetype.obj: arch\win32\freetype.c
	$(compile) $**
$(objdir)\ftxgasp.obj:  extend\ftxgasp.c
	$(compile) $**
$(objdir)\ftxgsub.obj:  extend\ftxgsub.c
	$(compile) $**
$(objdir)\ftxgpos.obj:  extend\ftxgpos.c
	$(compile) $**
$(objdir)\ftxgdef.obj:  extend\ftxgdef.c
	$(compile) $**
$(objdir)\ftxopen.obj:  extend\ftxopen.c
	$(compile) $**

!include <msvc/config.mak>
!include <msvc/install.mak>

install:: install-exec install-include install-lib

!include <msvc/clean.mak>

!include <msvc/rdepend.mak>
!include "./depend.mak"

#
# Local Variables:
# mode: makefile
# End:

