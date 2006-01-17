################################################################################
#
# Makefile  : Devnag / Src
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/08/04 15:53:03 popineau>
#
################################################################################
root_srcdir = ..\..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

# Makefile for devnag

USE_GNUW32 = 1

!include <msvc/common.mak>

DEFS = -I.. $(DEFS)

objects = $(objdir)\devnag.obj
program = $(objdir)\devnag.exe
programs = $(program)

default: all

all: $(objdir) $(programs)

$(program): $(objects) $(gnuw32lib)
	$(link) $(**) $(conlibs)

!include <msvc/config.mak>
!include <msvc/install.mak>

install:: install-exec

!include <msvc/clean.mak>

!include <msvc/rdepend.mak>
!include "./depend.mak"

#  
# Local variables:
# page-delimiter: "^# \f"
# mode: Makefile
# End:
