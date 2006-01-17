################################################################################
#
# Makefile  : Musixflx
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <01/11/19 14:55:29 popineau>
#
################################################################################
root_srcdir = ..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

USE_GNUW32 = 1
USE_KPATHSEA = 1

!include <msvc/common.mak>

DEFS = $(DEFS) -DKPATHSEA -DNeedFunctionPrototypes

program = $(objdir)\musixflx.exe
objects = $(objdir)\musixflx.obj

programs = $(program)

default: all

all: $(objdir) $(programs)

$(program): $(objects) $(kpathsealib)
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

