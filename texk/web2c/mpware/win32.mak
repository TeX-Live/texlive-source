################################################################################
#
# Makefile  : Web2C / mpware
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <03/02/21 11:53:59 popineau>
#
################################################################################
root_srcdir =  ..\..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

USE_KPATHSEA = 1
USE_GNUW32 = 1

!include <msvc/common.mak>

DEFS = -I.. $(DEFS) -DHAVE_CONFIG_H
proglib = ../lib/$(objdir)/lib.lib
programs = $(objdir)\dmp.exe $(objdir)\mpto.exe $(objdir)\newer.exe

default: all
all: $(objdir) $(programs) 

$(objdir)\dmp.exe: $(objdir)\dmp.obj $(objdir)\dmp.res $(proglib) $(kpathsealib)
	$(link) $(**) $(conlibs)
$(objdir)\mpto.exe: $(objdir)\mpto.obj $(objdir)\mpto.res $(proglib) $(kpathsealib)
	$(link) $(**) $(conlibs)
$(objdir)\newer.exe: $(objdir)\newer.obj $(proglib) $(gnuw32lib)
	$(link) $(**) $(conlibs)

$(objdir)\newer.obj: newer.c
	$(compile) -UKPSE_DLL newer.c

checkdir = $(srcdir)\..\tests
check: $(objdir)\mpto.exe $(objdir)\newer.exe $(objdir)\dmp.exe
	pushd $(root_srcdir)\texk\contrib & $(make) $(objdir)\makempx.exe & popd
	$(top_srcdir)\contrib\$(objdir)\makempx $(checkdir)/label.mp $(checkdir)/xlabel.mpx
	.\$(objdir)\mpto -tex $(checkdir)/testex.mp | diff $(checkdir)/testex.tex -
	$(sed) '16s/^%//' <$(checkdir)/testex.mp >testex1
	-.\$(objdir)\mpto -tex testex1 >nul 2>testex2
	$(sed) '17s/^%//' <$(checkdir)/testex.mp >testex1
	-.\$(objdir)\mpto -tex testex1 >nul 2>>testex2
	$(sed) '18s/^%//' <$(checkdir)/testex.mp >testex1
	-.\$(objdir)\mpto -tex testex1 >nul 2>>testex2
	$(sed) '19s/^%//' <$(checkdir)/testex.mp >testex1
	-.\$(objdir)\mpto -tex testex1 >nul 2>>testex2
	$(sed) '20s/^%//' <$(checkdir)/testex.mp >testex1
	-.\$(objdir)\mpto -tex testex1 >nul 2>>testex2
	$(diff) $(checkdir)/testex.err testex2

#Makefile: Makefile.in ../config.status
#	(cd ..; sh config.status)

!include <msvc/config.mak>
!include <msvc/install.mak>

install:: install-exec

install-programs: install-exec

!include <msvc/clean.mak>

clean::
	-@$(del) testex1 testex2

!include <msvc/rdepend.mak>
!include "./depend.mak"

# Local Variables:
# mode: Makefile
# End:
