################################################################################
#
# Makefile  : Detex
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/11/14 22:35:26 popineau>
#
################################################################################
root_srcdir = ..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif
#
# For BSD Unix use the following CFLAGS definition
# CFLAGS = -Dstrchr=index
#
# This Makefile does not work for MSDOS. Make your 
# own one, or compile by hand.
#

USE_KPATHSEA = 1
USE_GNUW32 = 1

!include <msvc/common.mak>

objects = $(objdir)\detex.obj
programs = $(objdir)\detex.exe
manfiles = detex.1

DEFS = $(DEFS) -DNOFILE=16

all: $(objdir) $(programs)

$(objdir)\detex.exe: $(objdir)\detex.obj $(kpathsealib)
	$(link) $(**) $(conlibs)

detex.c: xxx.l
	$(lex) -t xxx.l | $(sed) "/^extern int isatty YY/d" > detex.c

xxx.l: states.sed detex.l
	sed -f $(srcdir)/states.sed $(srcdir)/detex.l > xxx.l

!include <msvc/config.mak>
!include <msvc/install.mak>

install:: install-exec install-man

distclean::
	-$(del) xxx.l detex.c

!include <msvc/clean.mak>
!include <msvc/rdepend.mak>
!include "./depend.mak"

#
# Local Variables:
# mode: makefile
# End:
