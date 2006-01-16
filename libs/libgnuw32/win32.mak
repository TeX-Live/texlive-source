################################################################################
#
# Makefile  : Gnu Win32
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/10/25 23:50:48 popineau>
#
################################################################################
root_srcdir=..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

USE_GNUW32 = 1
MAKE_GNUW32 = 1

!include <msvc/common.mak>

{$(root_srcdir)/texk/kpathsea}.c{$(objdir)}.obj:
	$(compile) $<

DEFS = $(DEFS) -I$(kpathseadir)\..

objects = \
#	$(objdir)\dirent.obj   	\
	$(objdir)\error.obj	\
	$(objdir)\filename.obj 	\
	$(objdir)\getopt.obj	\
	$(objdir)\getopt1.obj	\
	$(objdir)\home.obj     	\
	$(objdir)\lookcmd.obj	\
	$(objdir)\popen.obj    	\
	$(objdir)\stat.obj     	\
	$(objdir)\string.obj   	\
	$(objdir)\system.obj   	\
	$(objdir)\time.obj     	\
	$(objdir)\user.obj     	\
	$(objdir)\volume.obj

programs = $(gnuw32dll)
libfiles = $(gnuw32lib)
includefiles = win32lib.h

default: all

all: $(objdir) $(gnuw32)

lib: $(objdir) $(gnuw32lib)

!ifdef GNUW32_DLL
DEFS= $(DEFS) -DMAKE_GNUW32_DLL

$(gnuw32lib): $(objdir) $(objects)
	$(archive) /DEF $(objects)

$(gnuw32dll): $(objects) $(objdir)\libgnuw32.lib  $(objdir)\libgnuw32.res
	$(link_dll) $(**) $(gnuw32lib:.lib=.exp) $(conlibs)
!else
$(gnuw32lib): $(objdir) $(objects)
	$(archive) $(objects)
!endif

check: $(objdir)\system_popen_check.exe

$(objdir)\system_popen_check.exe: $(objdir)\system_popen_check.obj $(gnuw32lib)
	$(link) $(**) $(conlibs)

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