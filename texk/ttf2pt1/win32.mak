################################################################################
#
# Makefile  : Ttf2pt1
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <03/01/19 10:45:23 popineau>
#
################################################################################
root_srcdir = ..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

USE_GNUW32 = 1
USE_TTF2PT1 = 1
USE_FREETYPE2 = 1

!include <msvc/common.mak>

programs = $(objdir)\ttf2pt1.exe
libfiles =
includefiles =
manfiles = ttf2pt1.1
infofiles =
installinfoflags =

objects= \
	$(objdir)\ttf2pt1.obj  \
	$(objdir)\pt1.obj    \
	$(objdir)\runt1asm.obj   \
	$(objdir)\ttf.obj  \
	$(objdir)\ft.obj   \
	$(objdir)\bdf.obj \
	$(objdir)\bitmap.obj

DEFS = $(DEFS) -DUSE_FREETYPE -DWINDOWS

default: all

all: $(objdir) $(programs)

$(objdir)\ttf2pt1.exe: $(objects) $(freetype2lib) $(gnuw32lib)
	$(link) $(**) $(conlibs)

!include <msvc/config.mak>
!include <msvc/install.mak>

makeinfo_flags = $(makeinfo_flags) --force

install:: install-exec install-man

!include <msvc/clean.mak>

!include <msvc/rdepend.mak>
!include "./depend.mak"

#
# Local Variables:
# mode: makefile
# End: