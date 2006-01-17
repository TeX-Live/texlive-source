################################################################################
#
# Makefile  : Gsftopk
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/02/28 21:06:26 popineau>
#
################################################################################
root_srcdir = ..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

version = 1.19.2

USE_GNUW32 = 1
USE_GSW32 = 1
USE_KPATHSEA = 1
USE_ADVAPI = 1

!include <msvc/common.mak>

# See INSTALL for compilation options.
DEFS = $(DEFS) -DHAVE_CONFIG_H

objects = $(objdir)\gsftopk.obj

programs = $(objdir)\gsftopk.exe

etcdir = $(psheaderdir)\gsftopk
etcfiles = render.ps

manfiles = gsftopk.1

default: all

all: $(objdir) $(programs)

$(objdir)\gsftopk.exe: $(objects) $(kpathsealib) $(gsw32lib)
	$(link) $(**) $(conlibs)

gsftopk.1: gsftopk1.sed sedscript.win32
	$(sed) -f $(srcdir)/sedscript.win32 <$(srcdir)/gsftopk1.sed >$@

sedscript.win32:
	@echo <<$@
/^#ifnokpathsea/,/^#/d
s/TEXFONTS/TFMFONTS/g
/^#/d
<<KEEP

!include <msvc/config.mak>
!include <msvc/install.mak>

install:: install-exec install-data

install-exec:: $(programs)

!include <msvc/clean.mak>

extraclean::
	-@$(del) -f sedscript.win32 gsftopk.1

!include <msvc/rdepend.mak>
!include "./depend.mak"

#
# Local Variables:
# mode: makefile
# End:
