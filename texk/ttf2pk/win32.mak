################################################################################
#
# Makefile  : Ttf2pk
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/03/13 16:35:14 popineau>
#
################################################################################
root_srcdir = ..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

USE_GNUW32 = 1
USE_KPATHSEA = 1
USE_TTF = 1

version = 1.2

!include <msvc/common.mak>

DEFS = $(DEFS) -DHAVE_LIBKPATHSEA

manfiles = ttf2tfm.1 ttf2pk.1

ttf2pkobjs = $(objdir)\errormsg.obj $(objdir)\filesrch.obj                  \
	$(objdir)\ligkern.obj $(objdir)\newobj.obj $(objdir)\parse.obj      \
	$(objdir)\pklib.obj $(objdir)\subfont.obj $(objdir)\texenc.obj      \
	$(objdir)\ttf2pk.obj $(objdir)\ttfenc.obj $(objdir)\ttflib.obj
ttf2tfmobjs = $(objdir)\case.obj $(objdir)\errormsg.obj                     \
	$(objdir)\filesrch.obj $(objdir)\ligkern.obj $(objdir)\newobj.obj   \
	$(objdir)\parse.obj $(objdir)\subfont.obj $(objdir)\texenc.obj      \
	$(objdir)\tfmaux.obj $(objdir)\ttf2tfm.obj $(objdir)\ttfaux.obj     \
	$(objdir)\ttfenc.obj $(objdir)\vplaux.obj

programs = $(objdir)\ttf2tfm.exe $(objdir)\ttf2pk.exe

default: all

all: $(objdir) $(programs)

$(objdir)\ttf2tfm.exe: $(ttf2tfmobjs) $(ttflib) $(kpathsealib)
	$(link) $(**) $(conlibs)

$(objdir)\ttf2pk.exe: $(ttf2pkobjs) $(ttflib) $(kpathsealib)
	$(link) $(**) $(conlibs)

!include <msvc/config.mak>

installdirs = $(texmf)\ttf2pk

!include <msvc/install.mak>

install:: install-exec install-data install-man

install-data::
	-@echo $(verbose) & ( \
		$(copydir) data\* $(texmf)\ttf2pk $(redir_stdout) \
	)

!include <msvc/clean.mak>
!include <msvc/rdepend.mak>
!include "./depend.mak"

#  
# Local variables:
# page-delimiter: "^# \f"
# mode: Makefile
# End:


