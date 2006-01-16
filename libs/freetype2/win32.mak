################################################################################
#
# Makefile  : Freetype
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/03/13 13:32:56 popineau>
#
################################################################################
root_srcdir = ..\..
INCLUDE=$(INCLUDE);$(root_srcdir)\texk

!include <msvc/common.mak>

# Package subdirectories, the library, and all subdirectories.
subdirs = \
	src

!include <msvc/subdirs.mak>
!include <msvc/clean.mak>

#
# Local Variables:
# mode: Makefile
# End:
# DO NOT DELETE
