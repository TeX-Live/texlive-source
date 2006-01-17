################################################################################
#
# Makefile  : ChkTeX
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <03/08/23 19:02:50 popineau>
#
################################################################################
root_srcdir = ..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

USE_KPATHSEA = 1
USE_GNUW32 = 1

!include <msvc/common.mak>

# We need to get getopt.h from kpathsea, not from the current directory.
DEFS = -I$(kpathseadir) -I. $(DEFS) -D_MSDOS -DHAVE_CONFIG_H

objects = $(objdir)\ChkTeX.obj $(objdir)\FindErrs.obj $(objdir)\OpSys.obj \
	$(objdir)\Resource.obj $(objdir)\Utility.obj

programs = $(objdir)\ChkTeX.exe

manfiles =
#infofiles = ChkTeX.info
docsubdir = chktex
pdfdocfiles = ChkTeX.pdf
htmlfiles =

installdirs = $(bindir) $(texmf)\chktex

default: all

all: $(objdir) $(programs)

$(objdir)\ChkTeX.exe: $(objects) $(kpathsealib)
	$(link) $(objects) $(kpathsealib) $(conlibs)

!include <msvc/config.mak>
!include <msvc/install.mak>

install:: install-exec install-data install-man install-doc

install-data::
	-@echo $(verbose) & ( \
		echo "Installing data in $(MAKEDIR)" & \
		$(copy) chktexrc $(texmf)\chktex\.chktexrc $(redir_stdout) \
	)

distclean::
	-@$(del) config.h chktex.dvi chktex.pdf

!include <msvc/clean.mak>
!include <msvc/rdepend.mak>
!include "./depend.mak"

#
# Local Variables:
# mode: makefile
# End: