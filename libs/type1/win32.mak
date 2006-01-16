################################################################################
#
# Makefile  : LibTYPE1
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/09/23 17:41:15 popineau>
#
################################################################################
root_srcdir = ..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

USE_GNUW32 = 1
USE_TYPE1 = 1

!include <msvc/common.mak>

programs = $(type1dll)
libfiles = $(type1lib)
includefiles = 
installdirs = $(bindir) $(includedir) $(libdir)

objects = \
	$(NULL)

default: all

all: $(objdir) $(type1)

lib: $(objdir) $(type1lib)

!ifdef TYPE1_DLL
DEFS = $(DEFS) -DMAKE_TYPE1_DLL

$(type1lib): $(objdir) $(objects)
	$(archive) /DEF $(objects)

$(type1dll): $(objects) $(objdir)\libtype1.res $(gnuw32lib) $(regexlib) $(zliblib)
	$(link_dll) $(**) $(type1lib:.lib=.exp) wsock32.lib $(conlibs)
!else
$(type1lib): $(objdir) $(objects) $(gnuw32lib) $(regexlib) $(zliblib)
	$(archive) $(gnuw32lib) $(regexlib) $(zliblib) wsock32.lib
!ENDIF

!include <msvc/config.mak>

!include <msvc/install.mak>

install:: install-exec install-include install-lib

!include <msvc/clean.mak>

!include <msvc/rdepend.mak>
!include "./depend.mak"

# 
# Local Variables:
# mode: makefile
# End:
