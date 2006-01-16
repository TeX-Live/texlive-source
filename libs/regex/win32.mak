################################################################################
#
# Makefile  : Regex
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/09/23 17:40:58 popineau>
#
################################################################################
root_srcdir=..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

USE_GNUW32 = 1
USE_REGEX = 1
MAKE_REGEX = 1

!include <msvc/common.mak>

DEFS= $(DEFS) -DHAVE_CONFIG_H

programs = $(regexdll)
libfiles = $(regexlib)
includefiles = regex.h
objects = $(objdir)\regex.obj

default: all

all: $(objdir) $(regex)

lib: $(objdir) $(regexlib)

!ifdef REGEX_DLL
DEFS = $(DEFS) -DMAKE_REGEX_DLL

$(regexlib): $(objdir) $(objects)
	$(archive) /DEF $(objects)

$(regexdll): $(objects) $(objdir)\regex.res
	$(link_dll) $(**) $(regexlib:.lib=.exp) $(conlibs)
!else
$(regexlib): $(objdir) $(objects)
	$(archive) $(objects)
!endif

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