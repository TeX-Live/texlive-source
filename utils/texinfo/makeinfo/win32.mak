################################################################################
#
# Makefile  : TeXInfo/makeinfo
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/11/14 20:21:20 popineau>
#
################################################################################
root_srcdir = ..\..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

GNU = 1
USE_GNUW32 = 1

!include <msvc/common.mak>

programs = $(objdir)\makeinfo.exe
objects = \
	$(objdir)\cmds.obj	\
	$(objdir)\defun.obj	\
	$(objdir)\files.obj	\
	$(objdir)\footnote.obj	\
	$(objdir)\html.obj	\
	$(objdir)\index.obj	\
	$(objdir)\insertion.obj \
	$(objdir)\lang.obj	\
	$(objdir)\macro.obj	\
	$(objdir)\makeinfo.obj	\
	$(objdir)\multi.obj	\
	$(objdir)\node.obj	\
	$(objdir)\sectioning.obj \
	$(objdir)\toc.obj	\
	$(objdir)\xml.obj

libtxi = ..\lib\$(objdir)\libtxi.lib

DEFS = -I.. -DHAVE_CONFIG_H $(DEFS) -I..\lib

default: all

all: $(objdir) $(programs)

$(objdir)\makeinfo.exe: $(objects) $(libtxi) $(gnuw32lib)
	$(link) $(**) $(conlibs)

$(libtxi):
	pushd ..\lib & $(make) $(makeflags) all & popd

!include <msvc/config.mak>
!include <msvc/install.mak>

install:: install-exec

!include <msvc/clean.mak>
!include <msvc/version.mak>

!include <msvc/rdepend.mak>
!include "./depend.mak"
#
# Local Variables:
# mode: makefile
# End: