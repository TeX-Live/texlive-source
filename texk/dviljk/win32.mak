################################################################################
#
# Makefile  : Dviljk
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/01/20 23:47:12 popineau>
#
################################################################################
root_srcdir = ..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

version = 2.6

USE_KPATHSEA = 1
USE_GNUW32 = 1

!include <msvc/common.mak>

DEFS = $(DEFS) -DHAVE_CONFIG_H

common_objects = $(objdir)\tfm.obj

program = dvilj.exe
objects = $(objdir)\dvilj.obj $(common_objects)
objects2p = $(objdir)\dvilj2p.obj $(common_objects)
program2p = dvilj2p.exe
objects4 = $(objdir)\dvilj4.obj $(common_objects)
program4 = dvilj4.exe
objects4l = $(objdir)\dvilj4l.obj $(common_objects)
program4l = dvilj4l.exe
program6 = dvilj6.exe

programs = $(objdir)\$(program) $(objdir)\$(program2p)  \
	$(objdir)\$(program4) $(objdir)\$(program4l)

manfiles = dvilj.1

#default: $(program4)
default: all

all: $(objdir) $(programs)

$(objdir)\$(program): $(objects) $(objdir)\dvilj.res $(kpathsealib)
	$(link) $(**) $(conlibs)
$(objdir)\$(program2p): $(objects2p) $(objdir)\dvilj.res $(kpathsealib)
	$(link) $(**) $(conlibs)
$(objdir)\$(program4): $(objects4) $(objdir)\dvilj.res $(kpathsealib)
	$(link) $(**) $(conlibs)
$(objdir)\$(program4l): $(objects4l) $(objdir)\dvilj.res $(kpathsealib)
	$(link) $(**) $(conlibs)

$(objdir)\dvilj.obj:
	$(compile) -DLJ $(srcdir)/dvi2xx.c
$(objdir)\dvilj2p.obj:
	$(compile) -DLJ2P $(srcdir)/dvi2xx.c
$(objdir)\dvilj4.obj:
	$(compile) -DLJ4 $(srcdir)/dvi2xx.c
$(objdir)\dvilj4l.obj:
	$(compile) -DLJ4 -DLJ4L $(srcdir)/dvi2xx.c

$(objdir)\dvilj.obj $(objdir)\dvilj2p.obj $(objdir)\dvilj4.obj $(objdir)\dvilj4l.obj: dvi2xx.c c-auto.h

!include <msvc/config.mak>

!include <msvc/install.mak>

install:: install-exec install-data install-man

install-exec:: $(programs)
	-$(del) $(bindir)\$(program6) & pushd $(bindir) \
	 & $(lnexe) .\$(program4) $(bindir)\$(program6) & popd

install-data::
#	$(INSTALL_DATA) $(srcdir)\dvilj.man $(mandir)\dvilj.$(manext)
#	cd $(srcdir)\tex && for %%m in (*.sty) do \
#	  $(INSTALL_DATA) %%m $(dvilj_latex2e_macrodir)\%%m

# It seems simplest to keep this separate. Use find and cpio instead of
# just cp -r in case the source hierarchy is cvs- or rcs-ified.
# install-fonts:
#	xcopy fonts $(texmf)\fonts /s
#	find fonts -name \*.tfm -o -name \*.vf -print | cpio -pld $(texmf)

!include <msvc/clean.mak>

!include <msvc/rdepend.mak>
!include "./depend.mak"

# Local Variables:
# mode: Makefile
# End:
