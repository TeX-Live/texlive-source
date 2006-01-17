################################################################################
#
# Makefile  : Bibtex8
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <04/02/16 17:43:20 popineau>
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

#
# Object files
#
objects = $(objdir)\bibtex-1.obj $(objdir)\bibtex-2.obj $(objdir)\bibtex-3.obj \
	$(objdir)\bibtex-4.obj $(objdir)\bibtex.obj $(objdir)\utils.obj

installdirs = $(bindir) $(texmf)\bibtex\csf\base $(texmf)\doc\bibtex8

etcfiles = csf\*.csf
etcdir = $(texmf)\bibtex\csf\base

# Beware : default char type is unsigned
DEFS = -I$(kpathseadir) $(DEFS) -DSUPPORT_8BIT -J
# for FAT support, set: DEFS = -DNTFAT

programs = $(objdir)\bibtex8.exe
#
# Main target
#
default: all

all: $(objdir) $(programs)

#
# Link target. setargv.obj is provided in the compiler library directory.
#
$(objdir)\bibtex8.exe: $(objects) $(kpathsealib)
	$(link) $(objects) $(kpathsealib) $(conlibs)

!include <msvc/config.mak>
!include <msvc/install.mak>


install:: all install-dirs install-exec install-data install-doc

install-doc::
	-@echo $(verbose) & ( \
		for %%f in (00readme.txt HISTORY csfile.txt file_id.diz) do \
			$(copy) %%f $(texmf)\doc\bibtex8\%%f \
	) $(redir_stdout)

!include <msvc/clean.mak>
!include <msvc/rdepend.mak>
!include "./depend.mak"

#
# Local Variables:
# mode: makefile
# End:
