################################################################################
#
# Makefile  : SeeTeXk
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/06/02 15:04:09 popineau>
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

!include <msvc/common.mak>

libobjects = $(objdir)\error.obj $(objdir)\fio.obj \
	$(objdir)\search.obj $(objdir)\seek.obj \
	$(objdir)\gripes0.obj $(objdir)\gripes1.obj \
	$(objdir)\dviclass.obj $(objdir)\tempfile.obj

objects = $(objdir)\dviconcat.obj $(objdir)\dviselect.obj \
	$(objdir)\dvibook.obj $(objdir)\dvitodvi.obj

programs = $(objdir)\dviconcat.exe $(objdir)\dviselect.exe \
	$(objdir)\dvibook.exe $(objdir)\dvitodvi.exe

manfiles = dviconcat.1 dviselect.1 dvibook.1 dvitodvi.1

default all: $(objdir) $(programs) 

$(objdir)\libtex.lib: $(libobjects)
	$(archive) $(libobjects)

$(objdir)\dviconcat.exe: $(objdir)\dviconcat.obj $(objdir)\libtex.lib $(kpathsealib)
	$(link) $(**) $(setargvobj) $(conlibs)

$(objdir)\dviselect.exe: $(objdir)\dviselect.obj $(objdir)\libtex.lib $(kpathsealib)
	$(link) $(**) $(conlibs)

$(objdir)\dvibook.exe: $(objdir)\dvibook.obj $(objdir)\libtex.lib $(kpathsealib)
	$(link) $(**) $(conlibs)

$(objdir)\dvitodvi.exe: $(objdir)\dvitodvi.obj $(objdir)\libtex.lib $(kpathsealib)
	$(link) $(**) $(conlibs)

# makedepend fails to find this one
$(objdir)\error.obj: c-auto.h

!include <msvc/config.mak>

!include <msvc/install.mak>

install:: install-exec

!include <msvc/clean.mak>


!include <msvc/rdepend.mak>
!include "./depend.mak"

#
# Local Variables:
# mode: makefile
# End:
