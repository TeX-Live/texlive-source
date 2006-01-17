################################################################################
#
# Makefile  : Web2C / window metafont support
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <01/12/14 19:41:51 popineau>
#
################################################################################
root_srcdir = ..\..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

USE_KPATHSEA = 1
USE_GNUW32 = 1

!include <msvc/common.mak>

DEFS = -I.. $(DEFS) -DHAVE_CONFIG_H
library = window

#objects = $(objdir)\amiga.obj $(objdir)\hp2627.obj $(objdir)\mftalk.obj \
#          $(objdir)\next.obj $(objdir)\regis.obj $(objdir)\suntools.obj \
#	   $(objdir)\tek.obj $(objdir)\trap.obj $(objdir)\uniterm.obj    \
#          $(objdir)\x11.obj 

objects = $(objdir)\epsf.obj $(objdir)\win32.obj $(objdir)\trap.obj

default: all

all: $(objdir) $(objdir)\$(library).lib

$(objdir)\$(library).lib: $(objects)
	$(archive) $(objects)

!include <msvc/config.mak>
!include <msvc/install.mak>

!include <msvc/clean.mak>

!include <msvc/rdepend.mak>
!include "./depend.mak"

# Local Variables:
# mode: Makefile
# End:
