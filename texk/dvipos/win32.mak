################################################################################
#
# Makefile  : Dvipos
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.7 sources
# Time-stamp: <03/02/26 15:51:47 popineau>
#
################################################################################
root_srcdir = ..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

# Makefile for ttfdump

USE_GNUW32 = 1
USE_KPATHSEA = 1

!include <msvc/common.mak>

DEFS = -I.. $(DEFS) -DHAVE_CONFIG_H \
	-I..\crypto
#	-DWITHOUT_OPENSSL
#	-DWITHOUT_TOUNICODE

manfiles =
objects = \
	$(objdir)\dvicore.obj \
	$(objdir)\dvipos.obj \
	$(objdir)\tfm.obj

program = $(objdir)\dvipos.exe
programs = $(program)

default: all

all: $(objdir) $(programs)

$(program): $(objects) $(kpathsealib)
	$(link) $(**) $(conlibs)

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
