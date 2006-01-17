################################################################################
#
# Makefile  : Ps2pkm
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <01/11/19 12:04:44 popineau>
#
################################################################################
root_srcdir = ..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

version = 1.5m

USE_GNUW32 = 1
USE_KPATHSEA = 1

!include <msvc/common.mak>

DEFS = $(DEFS) -DHAVE_CONFIG_H

libobj = $(objdir)\arith.obj $(objdir)\basics.obj $(objdir)\curves.obj    \
	$(objdir)\fontfcn.obj $(objdir)\hints.obj $(objdir)\lines.obj     \
	$(objdir)\objects.obj $(objdir)\paths.obj $(objdir)\regions.obj   \
	$(objdir)\scanfont.obj $(objdir)\spaces.obj $(objdir)\t1funcs.obj \
	$(objdir)\t1info.obj $(objdir)\t1io.obj $(objdir)\t1snap.obj      \
	$(objdir)\t1stub.obj $(objdir)\token.obj $(objdir)\type1.obj      \
	$(objdir)\util.obj # $(objdir)\bstring.obj

common_objects = $(objdir)\pkout.obj $(objdir)\filenames.obj
ps2pkobjs = $(libobj) $(objdir)\ps2pk.obj $(objdir)\encoding.obj $(objdir)\pkout.obj $(objdir)\filenames.obj
pk2bmobjs = $(objdir)\pk2bm.obj $(objdir)\pkin.obj $(objdir)\basics.obj
pfb2pfaobjs = $(objdir)\pfb2pfa.obj $(objdir)\filenames.obj $(objdir)\basics.obj
magobjs = $(objdir)\mag.obj $(objdir)\basics.obj

manfiles = mag.1 pfb2pfa.1 pk2bm.1 ps2pk.1

program = $(objdir)\ps2pk.exe
programs = $(objdir)\pfb2pfa.exe $(objdir)\mag.exe $(objdir)\pk2bm.exe $(program)

default all: $(objdir) $(programs)

$(program): $(ps2pkobjs) $(kpathsealib)
	$(link)  $(**) $(conlibs)
$(objdir)\pk2bm.exe: $(pk2bmobjs) $(kpathsealib)
	$(link) $(**) $(conlibs)
$(objdir)\pfb2pfa.exe: $(pfb2pfaobjs) $(kpathsealib)
	$(link) $(**) $(conlibs)
$(objdir)\mag.exe: $(magobjs) $(kpathsealib)
	$(link) $(**) $(conlibs)

!include <msvc/config.mak>

!include <msvc/install.mak>

install:: install-exec install-man

!include <msvc/clean.mak>

!include <msvc/rdepend.mak>
!include "./depend.mak"

# Local Variables:
# mode: Makefile
# End:
