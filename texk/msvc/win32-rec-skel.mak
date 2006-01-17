################################################################################
#
# Makefile  : recursive call on all subdirs
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/07/18 10:10:28 popineau>
#
################################################################################

root_srcdir=..
INCLUDE=$(INCLUDE);$(root_srcdir)\win32

subdirs = #

!include <msvc/common.mak>

default: all

!include <msvc/subdirs.mak>

!include <msvc/clean.mak>

# End of .mak
#
# Local Variables:
# mode: Makefile
# End:
# DO NOT DELETE
