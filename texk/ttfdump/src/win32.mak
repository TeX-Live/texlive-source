################################################################################
#
# Makefile  : TtfDump / Src
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <01/11/28 09:07:08 popineau>
#
################################################################################
root_srcdir = ..\..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

# Makefile for ttfdump

USE_GNUW32 = 1
USE_KPATHSEA = 1

!include <msvc/common.mak>

DEFS = -I.. -I../include -I../libttf $(DEFS) -UKPSE_DLL

manfiles = ..\docs\ttfdump.1
objects = $(objdir)\ttfdump.obj
libdump = ..\libttf\$(objdir)\libttf.lib
program = $(objdir)\ttfdump.exe
programs = $(program)

default: all

all: $(objdir) $(programs)

$(program): $(libdump) $(objects) $(gnuw32lib)
	$(link) $(**) $(conlibs)

$(libdump):
	pushd ..\libtff & $(make) all & popd

!include <msvc/config.mak>
!include <msvc/install.mak>

install:: install-exec install-man

!include <msvc/clean.mak>

!include <msvc/rdepend.mak>
!include "./depend.mak"

#  
# Local variables:
# page-delimiter: "^# \f"
# mode: Makefile
# End:
