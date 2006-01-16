################################################################################
#
# Makefile  : Curl / src
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/08/05 23:47:35 popineau>
#
################################################################################
root_srcdir = ..\..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

USE_GNUW32 = 1
USE_CURL = 1

!include <msvc/common.mak>

programs = $(objdir)\curl.exe
libfiles = 
includefiles = 
manfiles = 
infofiles =
installinfoflags =

objects= \
	$(objdir)\hugehelp.obj \
	$(objdir)\writeout.obj \
	$(objdir)\urlglob.obj \
	$(objdir)\main.obj

default: all

all: $(objdir) $(programs)

$(objdir)\curl.exe: $(objects) $(objdir)\curl.res $(curllib) $(gnuw32lib)
	$(link) $(**) $(conlibs)

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