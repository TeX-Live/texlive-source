################################################################################
#
# Makefile  : Jpeg2ps
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <01/11/20 13:37:41 popineau>
#
################################################################################
root_srcdir = ..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

USE_GNUW32 = 1

!include <msvc/common.mak>

DEFS = $(DEFS) -DA4 -I.

programs = $(objdir)\jpeg2ps.exe

objects = $(objdir)\jpeg2ps.obj $(objdir)\readjpeg.obj \
	$(objdir)\asc85ec.obj

default: all

all: $(objdir) $(programs)

$(objdir)\jpeg2ps.exe: $(objects) $(gnuw32lib)
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
