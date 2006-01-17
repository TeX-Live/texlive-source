################################################################################
#
# Makefile  : Vlna
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/06/03 15:10:30 popineau>
#
################################################################################
root_srcdir = ..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

USE_GNUW32 = 1

!include <msvc/common.mak>

programs = $(objdir)\vlna.exe
manfiles = vlna.1
infofiles = 
installinfoflags = 

objects= $(objdir)\vlna.obj

default: all

all: $(objdir) $(programs)

$(objdir)\vlna.exe: $(objdir)\vlna.obj
	$(link) $(**) $(conlibs)

!include <msvc/config.mak>
!include <msvc/install.mak>

vlna.1: vlna.man
	-@$(sed) -e "s|@BINDIR@|$(bindir)|;\
		     s|@MANDIR@|$(mandir)|;\
		     s|@MANEXT@|$(manext)|" vlna.man > vlna.$(manext)

install:: install-exec install-man


!include <msvc/clean.mak>

clean::
	-@$(del) sample1.rb2 sample2.rb2 sample1.tst sample2.tst

!include <msvc/rdepend.mak>
!include "./depend.mak"

#
# Local Variables:
# mode: makefile
# End: