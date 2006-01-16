################################################################################
#
# Makefile  : XPdf
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <03/08/14 19:05:00 popineau>
#
################################################################################
root_srcdir=..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

!include <msvc/common.mak>

# Package subdirectories.
subdirs = goo xpdf doc

!include <msvc/subdirs.mak>
!include <msvc/clean.mak>

distclean::
	-@$(del) aconf.h

#
# Local Variables:
# mode: Makefile
# End:
