################################################################################
#
# Makefile  : Freetype2 / src
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/09/23 17:44:10 popineau>
#
################################################################################
root_srcdir=..\..\..
INCLUDE=$(INCLUDE);$(root_srcdir)\texk

USE_GNUW32 = 1
USE_ZLIB = 1
USE_FREETYPE2 = 1
MAKE_FREETYPE2 = 1

!include <msvc/common.mak>

{../build/win32}.c{$(objdir)}.obj:
	$(compile) $<

{autohint}.c{$(objdir)}.obj:
	$(compile) $<

{cff}.c{$(objdir)}.obj:
	$(compile) $<

{base}.c{$(objdir)}.obj:
	$(compile) $<

{pcf}.c{$(objdir)}.obj:
	$(compile) $<

{psaux}.c{$(objdir)}.obj:
	$(compile) $<

{pshinter}.c{$(objdir)}.obj:
	$(compile) $<

{psnames}.c{$(objdir)}.obj:
	$(compile) $<

{raster}.c{$(objdir)}.obj:
	$(compile) $<

{sfnt}.c{$(objdir)}.obj:
	$(compile) $<

{smooth}.c{$(objdir)}.obj:
	$(compile) $<

{truetype}.c{$(objdir)}.obj:
	$(compile) $<

{type1}.c{$(objdir)}.obj:
	$(compile) $<

{cid}.c{$(objdir)}.obj:
	$(compile) $<

{winfonts}.c{$(objdir)}.obj:
	$(compile) $<

programs = $(freetype2dll)
libfiles = $(freetype2lib)
includefiles = 
manfiles =
objects = \
	$(objdir)\ftdebug.obj	\
	$(objdir)\autohint.obj	\
	$(objdir)\cff.obj	\
	$(objdir)\ftbase.obj	\
	$(objdir)\ftglyph.obj	\
	$(objdir)\ftinit.obj	\
	$(objdir)\ftmm.obj	\
	$(objdir)\ftsystem.obj	\
	$(objdir)\pcf.obj	\
	$(objdir)\psaux.obj	\
	$(objdir)\pshinter.obj	\
	$(objdir)\psmodule.obj	\
	$(objdir)\raster.obj	\
	$(objdir)\sfnt.obj	\
	$(objdir)\smooth.obj	\
	$(objdir)\truetype.obj	\
	$(objdir)\type1.obj	\
	$(objdir)\type1cid.obj	\
	$(objdir)\winfnt.obj

default: all

all: $(objdir) $(freetype2)

lib: $(objdir) $(freetype2lib)

!ifdef FREETYPE2_DLL
DEFS = $(DEFS) -DMAKE_FREETYPE2_DLL

$(freetype2lib):  $(objects)
	$(archive) /DEF $(objects)

$(freetype2dll): $(objects) $(gnuw32lib)
	$(link_dll) $(objects) $(freetype2lib:.lib=.exp) $(conlibs) $(gnuw32lib)
!else
$(freetype2lib): $(objects)
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
