################################################################################
#
# Makefile  : Freetype / src
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <03/08/12 23:08:48 popineau>
#
################################################################################
root_srcdir=..\..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

USE_GNUW32 = 1
USE_ZLIB = 1
USE_FREETYPE = 1
MAKE_FREETYPE = 1

!include <msvc/common.mak>

programs = $(freetypedll)
libfiles = $(freetypelib)
includefiles = 
manfiles =
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
	$(objdir)\ttraster.obj

default: all

all: $(objdir) $(freetype)

lib: $(objdir) $(freetypelib)

!ifdef FREETYPE_DLL
DEFS = $(DEFS) -I.\arch\win32 -DMAKE_FREETYPE_DLL

$(freetypelib):  $(objects)
	$(archive) /DEF $(objects)

$(freetypedll): $(objects) $(gnuw32lib)
	$(link_dll) $(objects) $(freetypelib:.lib=.exp) $(conlibs) $(gnuw32lib)
!else
$(freetypelib): $(objects)
	$(archive) $(objects)
!endif

!include <msvc/config.mak>
!include <msvc/install.mak>

install:: install-exec install-lib install-include

!include <msvc/clean.mak>

!include <msvc/rdepend.mak>
!include "./depend.mak"

#
# Local Variables:
# mode: makefile
# End:
