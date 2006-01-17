################################################################################
#
# Makefile  : Web2C / doc
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <03/02/19 16:24:17 popineau>
#
################################################################################
root_srcdir = ..\..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

!include <msvc/common.mak>

infofiles = web2c.info
pdfdocfiles = web2c.pdf
manfiles =
htmldocfiles = web2c.html

docsubdir = web2c

all: doc

info: doc

!include <msvc/config.mak>
!include <msvc/install.mak>

install:: install-info install-doc

web2c.info: install.texi ref.txi

!include <msvc/clean.mak>

#
# Local Variables:
# mode: makefile
# End:

