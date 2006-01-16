################################################################################
#
# Makefile  : XPdf/Goo library
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/06/07 00:07:59 popineau>
#
################################################################################
root_srcdir = ..\..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

USE_GNUW32 = 1

!include <msvc/common.mak>

objects = \
	$(objdir)\GHash.obj	\
	$(objdir)\GList.obj	\
	$(objdir)\GString.obj	\
	$(objdir)\gmempp.obj	\
	$(objdir)\gfile.obj	\
	$(objdir)\gmem.obj	\
	$(objdir)\parseargs.obj

goo = $(objdir)\libGoo.lib

DEFS = $(DEFS) -I..

default: all

all: $(objdir) $(objdir)\libGoo.lib

lib: $(objdir) $(objdir)\libGoo.lib

$(objdir)\libGoo.lib: $(objdir) $(objects) # libGoo.def
	$(archive) $(objects)

..\aconf.h: ..\aconf.h.in
	$(perl) $(win32perldir)\conf-cauto.pl ..\aconf.h.in ..\aconf.h

!include <msvc/config.mak>
!include <msvc/install.mak>

install::

!include <msvc/clean.mak>
!include <msvc/rdepend.mak>
!include "./depend.mak"

#
# Local Variables:
# mode: makefile
# End:
