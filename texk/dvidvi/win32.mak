################################################################################
#
# Makefile  : Dvi2tty
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <01/11/19 11:01:31 popineau>
#
################################################################################
root_srcdir = ..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

USE_KPATHSEA = 1
USE_GNUW32 = 1

!include <msvc/common.mak>

DEFS = $(DEFS) -DHAVE_CONFIG_H

manfiles = dvidvi.1
programs = $(objdir)\dvidvi.exe
scripts = a5bookle.bat doubside.bat

default all: $(objdir) $(programs)

$(objdir)\dvidvi.exe: $(objdir)\dvidvi.obj $(kpathsealib)
	$(link) $(**) $(conlibs)

!include <msvc/config.mak>
!include <msvc/install.mak>

install:: install-exec install-man

!include <msvc/clean.mak>
!include <msvc/rdepend.mak>
!include "./depend.mak"

#
# Local Variables:
# mode: makefile
# End: