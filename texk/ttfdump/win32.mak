################################################################################
#
# Makefile  : TTFDump
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/01/18 23:27:01 popineau>
#
################################################################################
root_srcdir = ..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

!include <msvc/common.mak>

# Package subdirectories, the library, and all subdirectories.
subdirs = libttf src

default:: all

all:: config.h

!include <msvc/subdirs.mak>
!include <msvc/config.mak>
!include <msvc/clean.mak>

# Local Variables:
# mode: Makefile
# End:
