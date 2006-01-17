################################################################################
#
# Makefile  : Windvi / doc
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <04/03/05 12:04:00 popineau>
#
################################################################################
root_srcdir = ..\..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

pdfdocfiles = windvi.pdf
manfiles =
htmldocfiles = windvi.html

docsubdir = windvi

!include <msvc/common.mak>

all: doc

info: doc

!include <msvc/config.mak>
!include <msvc/install.mak>

install:: install-info install-doc

!include <msvc/clean.mak>

#
# Local Variables:
# mode: makefile
# End:

