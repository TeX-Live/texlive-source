################################################################################
#
# Makefile  : LibPNG
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <03/01/28 21:36:51 popineau>
#
################################################################################
root_srcdir = ..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

USE_PNG = 1
MAKE_PNG = 1
USE_ZLIB = 1

!include <msvc/common.mak>

programs = $(pngdll)
manfiles = libpng.3 libpngpf.3 png.5
includefiles = png.h pngconf.h
libfiles = $(pnglib)
installdirs = $(installdirs) $(man3dir) $(man5dir)
objects = $(objdir)\png.obj $(objdir)\pngset.obj $(objdir)\pngget.obj        \
	$(objdir)\pngrutil.obj $(objdir)\pngtrans.obj $(objdir)\pngwutil.obj \
	$(objdir)\pngread.obj $(objdir)\pngrio.obj $(objdir)\pngwio.obj      \
	$(objdir)\pngwrite.obj $(objdir)\pngrtran.obj $(objdir)\pngwtran.obj \
	$(objdir)\pngmem.obj $(objdir)\pngerror.obj $(objdir)\pngpread.obj   \
	$(objdir)\pngvcrd.obj

default: all

all: $(objdir) $(png)

lib: $(objdir) $(png)

rcflags = $(rcflags) /l 0x409 /d "NDEBUG" /d "PNG_USE_PNGVCRD" /d PNG_VERSION_INFO_ONLY

!ifdef PNG_DLL
DEFS= $(DEFS) -DPNG_BUILD_DLL

$(pnglib): $(objdir) $(objects)
	$(archive) /DEF $(objects)

$(pngdll): $(objects) $(zliblib) $(objdir)\libpng.res $(gnuw32lib)
	$(link_dll) $(**) $(pnglib:.lib=.exp) $(conlibs)
!else
$(pnglib): $(objdir) $(objects) $(zliblib)
	$(archive) $(objects) $(zliblib)
!endif

$(objdir)\pngtest.exe: $(objdir)\pngtest.obj $(pnglib)
	$(link) /OUT:$@ $(objdir)\pngtest.obj $(pnglib) $(conlibs)

$(objdir)\pngtest.obj: pngtest.c
	$(compile) -UBUILD_PNG_DLL pngtest.c 

test: $(objdir)\pngtest.exe
	.\$(objdir)\pngtest.exe

!include <msvc/config.mak>

!include <msvc/install.mak>

install:: install-exec install-include install-lib

!include <msvc/clean.mak>

clean::
	-@$(del) pngout.png

!include <msvc/rdepend.mak>
!include "./depend.mak"

# 
# Local Variables:
# mode: makefile
# End:
