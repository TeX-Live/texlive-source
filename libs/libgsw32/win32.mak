################################################################################
#
# Makefile  : Gs Win32
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <03/09/15 12:11:00 popineau>
#
################################################################################
root_srcdir=..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

!include <msvc/common.mak>

USE_GNUW32 = 1
USE_GSW32 = 1
MAKE_GSW32 = 1
USE_ADVAPI = 1
USE_KPATHSEA = 1

!include <msvc/common.mak>

objects = \
	$(objdir)\gvwgsver.obj \
	$(objdir)\ghostscr.obj 

programs = $(gsw32dll)
libfiles = $(gsw32lib)
includefiles = 

DEFS = $(DEFS) -I..\..\gstools\ghostscript\src

default: all

all: $(objdir) $(gsw32)

lib: $(objdir) $(gsw32lib)

!ifdef GSW32_DLL
DEFS= $(DEFS) -DMAKE_GSW32_DLL

$(gsw32lib): $(objdir) $(objects)
	$(archive) /DEF $(objects)

$(gsw32dll): $(objects) $(objdir)\libgsw32.lib  $(objdir)\libgsw32.res
	$(link_dll) $(**) $(gsw32lib:.lib=.exp) $(conlibs)
!else
$(gsw32lib): $(objdir) $(objects)
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