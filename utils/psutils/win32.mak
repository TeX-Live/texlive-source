################################################################################
#
# Makefile  : Psutils
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/12/25 22:26:52 popineau>
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

PAPER=a4
OS = Win32

DEFS = -I. $(DEFS) -DPAPER=\"$(PAPER)\" -D$(OS)

programs = $(objdir)\psbook.exe $(objdir)\psselect.exe $(objdir)\pstops.exe \
	$(objdir)\epsffit.exe $(objdir)\psnup.exe \
	$(objdir)\psresize.exe
shellscripts =  
perlscripts = fixfmps.bat fixmacps.bat fixpsditps.bat fixpspps.bat \
	fixtpps.bat fixwfwps.bat fixwpps.bat fixscribeps.bat fixwwps.bat \
	fixdlsrps.bat extractres.bat includeres.bat psmerge.bat
scripts = $(shellscripts) $(perlscripts)
etcfiles = md68_0.ps md71_0.ps
etcdir = $(psheaderdir)\psutils
perlcmd=perl

manpages = psbook.man psselect.man pstops.man epsffit.man psnup.man psresize.man \
	$(perlscripts:.bat=.man)

mansedscript = mansedscript.sed
manfiles = $(manpages:.man=.1)

default: all

all: $(objdir) $(programs) $(perlscripts)

$(objdir)\epsffit.exe: $(objdir)\epsffit.obj $(objdir)\pserror.obj
	$(link) $(**) $(conlibs)

$(objdir)\psnup.exe: $(objdir)\psnup.obj $(objdir)\psutil.obj $(objdir)\psspec.obj $(objdir)\pserror.obj
	$(link) $(**) $(conlibs)

$(objdir)\psresize.exe: $(objdir)\psresize.obj $(objdir)\psutil.obj $(objdir)\pserror.obj $(objdir)\psspec.obj
	$(link) $(**) $(conlibs)

$(objdir)\psbook.exe: $(objdir)\psbook.obj $(objdir)\psutil.obj $(objdir)\pserror.obj
	$(link) $(**) $(conlibs)

$(objdir)\psselect.exe: $(objdir)\psselect.obj $(objdir)\psutil.obj $(objdir)\pserror.obj
	$(link) $(**) $(conlibs)

$(objdir)\pstops.exe: $(objdir)\pstops.obj $(objdir)\psutil.obj $(objdir)\psspec.obj $(objdir)\pserror.obj
	$(link) $(**) $(conlibs)

psmerge.bat: psmerge.pl
	$(perl) maketext OS=$(OS) perl=$(perlcmd) $? > $@

fixfmps.bat: fixfmps.pl
	$(perl) maketext OS=$(OS) perl=$(perlcmd) $? > $@

fixmacps.bat: fixmacps.pl
	$(perl) maketext OS=$(OS) perl=$(perlcmd) INCLUDE=$(INCLUDEDIR) $? > $@

fixpsditps.bat: fixpsditps.pl
	$(perl) maketext OS=$(OS) perl=$(perlcmd) $? > $@

fixpspps.bat: fixpspps.pl
	$(perl) maketext OS=$(OS) perl=$(perlcmd) $? > $@

fixscribeps.bat: fixscribeps.pl
	$(perl) maketext OS=$(OS) perl=$(perlcmd) $? > $@

fixtpps.bat: fixtpps.pl
	$(perl) maketext OS=$(OS) perl=$(perlcmd) $? > $@

fixwfwps.bat: fixwfwps.pl
	$(perl) maketext OS=$(OS) perl=$(perlcmd) $? > $@

fixwpps.bat: fixwpps.pl
	$(perl) maketext OS=$(OS) perl=$(perlcmd) $? > $@

fixwwps.bat: fixwwps.pl
	$(perl) maketext OS=$(OS) perl=$(perlcmd) $? > $@

fixdlsrps.bat: fixdlsrps.pl
	$(perl) maketext OS=$(OS) perl=$(perlcmd) $? > $@

extractres.bat: extractres.pl
	$(perl) maketext OS=$(OS) perl=$(perlcmd) $? > $@

includeres.bat: includeres.pl
	$(perl) maketext OS=$(OS) perl=$(perlcmd) INCLUDE=$(INCLUDEDIR) $? > $@

epsffit.$(manext): epsffit.man
	$(perl) maketext MAN="$(manpages:.man=.1)" $*.man > $@

psnup.$(manext): psnup.man
	$(perl) maketext MAN="$(manpages:.man=.1)" PAPER=$(PAPER) $*.man > $@

psresize.$(manext): psresize.man
	$(perl) maketext MAN="$(manpages:.man=.1)" PAPER=$(PAPER) $*.man > $@

psbook.$(manext): psbook.man
	$(perl) maketext "MAN=$(manpages:.man=.1)" $*.man > $@

psselect.$(manext): psselect.man
	$(perl) maketext "MAN=$(manpages:.man=.1)" $*.man > $@

pstops.$(manext): pstops.man
	$(perl) maketext "MAN=$(manpages:.man=.1)" PAPER=$(PAPER) $*.man > $@

psmerge.$(manext): psmerge.man
	$(perl) maketext "MAN=$(manpages:.man=.1)" $*.man > $@

fixfmps.$(manext): fixfmps.man
	$(perl) maketext "MAN=$(manpages:.man=.1)" $*.man > $@

fixmacps.$(manext): fixmacps.man
	$(perl) maketext "MAN=$(manpages:.man=.1)" INCLUDE=$(INCLUDEDIR) $*.man > $@

fixpsditps.$(manext): fixpsditps.man
	$(perl) maketext "MAN=$(manpages:.man=.1)" $*.man > $@

fixpspps.$(manext): fixpspps.man
	$(perl) maketext "MAN=$(manpages:.man=.1)" $*.man > $@

fixscribeps.$(manext): fixscribeps.man
	$(perl) maketext "MAN=$(manpages:.man=.1)" $*.man > $@

fixtpps.$(manext): fixtpps.man
	$(perl) maketext "MAN=$(manpages:.man=.1)" $*.man > $@

fixwfwps.$(manext): fixwfwps.man
	$(perl) maketext "MAN=$(manpages:.man=.1)" $*.man > $@

fixwpps.$(manext): fixwpps.man
	$(perl) maketext "MAN=$(manpages:.man=.1)" $*.man > $@

fixwwps.$(manext): fixwwps.man
	$(perl) maketext "MAN=$(manpages:.man=.1)" $*.man > $@

fixdlsrps.$(manext): fixdlsrps.man
	$(perl) maketext "MAN=$(manpages:.man=.1)" $*.man > $@

extractres.$(manext): extractres.man
	$(perl) maketext "MAN=$(manpages:.man=.1)" $*.man > $@

includeres.$(manext): includeres.man
	$(perl) maketext "MAN=$(manpages:.man=.1)" INCLUDE=$(INCLUDEDIR) $*.man > $@

#getafm.$(manext): getafm.man
#	$(perl) maketext "MAN=$(manpages:.man=.1)" $*.man > $@

patchlev.h: patchlev.h.in
	$(perl) $(win32perldir)/conf-cauto.pl $(**) $@

!include <msvc/config.mak>

installdirs = $(bindir) $(includes) $(psheaderdir)\psutils

!include <msvc/install.mak>

install::  install-exec install-data install-man

mansedscript.sed:
	@echo <<mansedscript.sed
1,$$s/@PAPER@/A4/g
1,$$s/@RELEASE@/1/g
1,$$s/@PATCHLEVEL@/17/g
1,$$s!@INCLUDE@!$(psheaderdir:\=/)/psutils!g
<<KEEP

!include <msvc/clean.mak>

distclean::
	-@$(del) $(perlscripts) $(perlscripts:.bat=.cmd) patchlev.h
	-@$(del) $(manfiles) mansedscript.sed

!include <msvc/rdepend.mak>
!include "./depend.mak"

#
# Local Variables:
# mode: makefile
# End: