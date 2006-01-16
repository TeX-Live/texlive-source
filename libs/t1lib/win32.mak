################################################################################
#
# Makefile  : LibT1
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/09/23 17:41:08 popineau>
#
################################################################################
root_srcdir = ..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

USE_GNUW32 = 1
USE_T1 = 1

!include <msvc/common.mak>

programs = $(t1dll)
libfiles = $(t1lib)
includefiles = 
installdirs = $(bindir) $(includedir) $(libdir)

objects = \
	$(objdir)\parseAFM.obj \
	$(objdir)\t1aaset.obj \
	$(objdir)\t1afmtool.obj \
	$(objdir)\t1base.obj \
	$(objdir)\t1delete.obj \
	$(objdir)\t1enc.obj \
	$(objdir)\t1env.obj \
	$(objdir)\t1finfo.obj \
	$(objdir)\t1load.obj \
	$(objdir)\t1outline.obj \
	$(objdir)\t1set.obj \
	$(objdir)\t1subset.obj \
	$(objdir)\t1trans.obj \
	$(objdir)\t1win32.obj \
	$(NULL)

default: all

all: $(objdir) $(t1)

lib: $(objdir) $(t1lib)

!ifdef T1_DLL
DEFS = $(DEFS) -DMAKE_T1_DLL

$(t1lib): $(objdir) $(objects)
	$(archive) /DEF $(objects)

$(t1dll): $(objects) $(objdir)\libt1.res $(gnuw32lib) $(regexlib) $(zliblib)
	$(link_dll) $(**) $(t1lib:.lib=.exp) wsock32.lib $(conlibs)
!else
$(t1lib): $(objdir) $(objects) $(gnuw32lib) $(regexlib) $(zliblib)
	$(archive) $(**) wsock32.lib
!ENDIF

!include <msvc/config.mak>

!include <msvc/install.mak>

install:: install-exec install-include install-lib

!include <msvc/clean.mak>

!include <msvc/rdepend.mak>
!include "./depend.mak"

# 
# Local Variables:
# mode: makefile
# End:
