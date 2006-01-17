################################################################################
#
# Makefile  : Web2C / library
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/10/26 18:58:43 popineau>
#
################################################################################
root_srcdir=..\..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

USE_KPATHSEA = 1
USE_GNUW32 = 1
USE_TEX = 1

!include <msvc/common.mak>

DEFS = -I.. $(DEFS) -DMAKE_TEX_DLL -DHAVE_CONFIG_H -DOEM \
	-DJOBTIME -DTIME_STATS \
	-DOUTPUT_DIR -DHALT_ON_ERROR

library = lib

# We don't make `texmfmp.o', since TeX, Metafont, and MetaPost need to
# use different routine names, hence they need different .o files. Maybe
# we should change this via #define's someday ...
objects = \
	$(objdir)\basechsuffix.obj	\
	$(objdir)\oem.obj		\
	$(objdir)\chartostring.obj	\
	$(objdir)\coredump.obj		\
	$(objdir)\eofeoln.obj		\
	$(objdir)\fprintreal.obj	\
	$(objdir)\inputint.obj		\
	$(objdir)\input2int.obj		\
	$(objdir)\main.obj		\
	$(objdir)\openclose.obj		\
	$(objdir)\printversion.obj	\
	$(objdir)\uexit.obj		\
	$(objdir)\usage.obj		\
	$(objdir)\version.obj		\
	$(objdir)\zround.obj

default: all

all: $(objdir) $(objdir)\$(library).lib

$(objdir)\$(library).lib: $(objects)
	$(archive) $(objects)

!include <msvc/install.mak>
!include <msvc/clean.mak>
!include <msvc/rdepend.mak>
!include "./depend.mak"

# Local Variables:
# mode: Makefile
# End:
