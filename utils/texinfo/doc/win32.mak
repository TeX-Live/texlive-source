################################################################################
#
# Makefile  : TeXInfo / Doc
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <03/08/14 21:22:55 popineau>
#
################################################################################
root_srcdir = ..\..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

# Copyright (C) 1994, 1995, 1996, 1997 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy, distribute and modify it.

!include <msvc/common.mak>

default: all

all:

infofiles = info-stnd.info info.info texinfo.info
pdfdocfiles = info-stnd.pdf info.pdf texinfo.pdf
htmldocfiles = info-stnd.html info.html texinfo.html
docsubdir = texinfo

install:: install-doc

depend::

!include <msvc/install.mak>
!include <msvc/clean.mak>

#
# Local Variables:
# mode: makefile
# End: