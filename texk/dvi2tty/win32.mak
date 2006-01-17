################################################################################
#
# Makefile  : Dvi2tty
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/02/02 00:37:46 popineau>
#
################################################################################
root_srcdir = ..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif
#
# For BSD Unix use the following CFLAGS definition
# CFLAGS = -Dstrchr=index
#
# This Makefile does not work for MSDOS. Make your 
# own one, or compile by hand.
#

USE_KPATHSEA = 1
USE_GNUW32 = 1

!include <msvc/common.mak>

objects = $(objdir)\dvi2tty.obj $(objdir)\disdvi.obj
programs = $(objdir)\dvi2tty.exe $(objdir)\disdvi.exe
manfiles = dvi2tty.1

all: $(objdir) $(programs)

$(objdir)\dvi2tty.exe: $(objdir)\dvi2tty.obj $(objdir)\dvistuff.obj $(kpathsealib)
	$(link) $(**) $(conlibs)

$(objdir)\disdvi.exe: $(objdir)\disdvi.obj $(kpathsealib)
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
