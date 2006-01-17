################################################################################
#
# Makefile  : MakeIndex(k)
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <01/11/19 11:36:09 popineau>
#
################################################################################
root_srcdir = ..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

version = 2.13

USE_KPATHSEA = 1
USE_GNUW32 = 1

!include <msvc/common.mak>

DEFS = $(DEFS) -DHAVE_CONFIG_H

objects = \
	$(objdir)\genind.obj \
	$(objdir)\mkind.obj \
	$(objdir)\qsort.obj \
	$(objdir)\scanid.obj \
	$(objdir)\scanst.obj \
	$(objdir)\sortid.obj
program = $(objdir)\makeindex.exe
programs = $(program)

manfiles = makeindex.1

default all: $(objdir) $(programs)

$(program): $(objects) $(objdir)\makeindex.res $(kpathsealib)
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
