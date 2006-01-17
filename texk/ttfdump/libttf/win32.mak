################################################################################
#
# Makefile  : TtfDump / Lib TTF
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/01/21 12:08:28 popineau>
#
################################################################################
root_srcdir = ..\..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

USE_GNUW32 = 1
USE_KPATHSEA = 1

!include <msvc/common.mak>

DEFS = -I..\include -I. -I.. $(DEFS)

objects = 	$(objdir)\ttfread.obj $(objdir)\tabledir.obj $(objdir)\ttfutil.obj  \
	$(objdir)\font.obj $(objdir)\cmap.obj $(objdir)\glyf.obj            \
	$(objdir)\gcache.obj $(objdir)\head.obj $(objdir)\hhea.obj          \
	$(objdir)\hmtx.obj $(objdir)\loca.obj $(objdir)\maxp.obj            \
	$(objdir)\name.obj $(objdir)\os2.obj $(objdir)\post.obj             \
	$(objdir)\cvt.obj $(objdir)\fpgm.obj $(objdir)\gasp.obj             \
	$(objdir)\hdmx.obj $(objdir)\kern.obj $(objdir)\ltsh.obj            \
	$(objdir)\prep.obj $(objdir)\pclt.obj $(objdir)\vdmx.obj            \
	$(objdir)\vhea.obj $(objdir)\vmtx.obj $(objdir)\gstate.obj          \
	$(objdir)\stack.obj $(objdir)\interp.obj $(objdir)\disasm.obj       \
	$(objdir)\ttc.obj

default: all

all: $(objdir) $(objdir)\libttf.lib

$(objdir)\libttf.lib: $(objects)
	$(archive) $(objects)

..\config.h:
	pushd .. & $(make) config.h & popd

!include <msvc/config.mak>
!include <msvc/install.mak>

!include <msvc/clean.mak>
!include <msvc/rdepend.mak>
!include "./depend.mak"

#
# Local Variables:
# mode: makefile
# End:


