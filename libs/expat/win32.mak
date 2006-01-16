################################################################################
#
# Makefile  : Expat
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <01/11/19 09:44:28 popineau>
#
################################################################################
# Top-level Makefile for kpathsea-using programs.
root_srcdir = ..\..
INCLUDE=$(INCLUDE);$(root_srcdir)\texk

!include <msvc/common.mak>

# Package subdirectories.
subdirs = lib

!include <msvc/subdirs.mak>
!include <msvc/clean.mak>

# Local Variables:
# mode: Makefile
# End:
