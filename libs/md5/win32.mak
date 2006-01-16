################################################################################
#
# Makefile  : Md5
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <03/07/22 05:14:54 popineau>
#
################################################################################
root_srcdir=..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

USE_MD5 = 1
MAKE_MD5 = 1
USE_GNUW32 = 1

!include <msvc/common.mak>

programs = $(md5dll)
libfiles = $(md5lib)
includefiles = md5.h
manfiles =
objects = \
	$(objdir)\md5.obj

default: all

all: $(objdir) $(md5) $(programs)

lib: $(objdir) $(md5)

!ifdef MD5_DLL
DEFS = $(DEFS) -DMAKE_MD5_DLL

md5def = $(library_prefix)md5.def

$(md5def): md5.def
	sed -e "/^NAME/s/^.*$$/NAME $(library_prefix)md5.dll/" < $(**) > $@

$(md5lib): $(md5def) $(objects)
	$(archive) /DEF $(objects)

$(md5dll): $(objects) $(objdir)\md5.res $(gnuw32lib)
	$(link_dll) $(objects) $(md5lib:.lib=.exp) $(objdir)\md5.res $(conlibs) $(gnuw32lib)
!else
$(md5lib): $(objects)
	$(archive) $(objects)
!endif

!include <msvc/config.mak>
!include <msvc/install.mak>

install:: install-exec install-lib install-include

!include <msvc/clean.mak>

!include <msvc/rdepend.mak>
!include "./depend.mak"

#
# Local Variables:
# mode: makefile
# End:
