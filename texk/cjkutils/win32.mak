################################################################################
#
# Makefile  : TeXk / CJKutils
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <05/08/18 00:00:00 wl>
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
subdirs = bat conv hbf2gf

!include <msvc/subdirs.mak>
!include <msvc/clean.mak>

# Local Variables:
# mode: Makefile
# End:
