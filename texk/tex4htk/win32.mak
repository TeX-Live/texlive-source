################################################################################
#
# Makefile  : TeX4ht(k)
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <04/03/20 00:36:48 popineau>
#
################################################################################
root_srcdir = ..\..
INCLUDE=$(INCLUDE);$(root_srcdir)\texk

version = 1.15

USE_KPATHSEA = 1
USE_GNUW32 = 1

!include <msvc/common.mak>

# See INSTALL for compilation options.
DEFS = $(DEFS) -DANSI

objects = $(objdir)\tex4ht.obj

programs = $(objdir)\tex4ht.exe $(objdir)\t4ht.exe $(objdir)\runht.exe

perlscripts = htperl.pl # mk4ht.pl

default: all

all: $(objdir) $(programs) # tex4ht.bat

$(objdir)\tex4ht.exe: $(objdir)\tex4ht.obj $(kpathsealib)
	$(link) $(**) $(conlibs)

$(objdir)\t4ht.exe: $(objdir)\t4ht.obj $(kpathsealib)
	$(link) $(**) $(conlibs)

$(objdir)\runht.exe: $(objdir)\runht.obj $(kpathsealib)
	$(link) $(**) $(conlibs)

tex4ht.env: tex4ht.in
	sed -e "s;@texmf@;$(texmf:\=\\);" < tex4ht.in \
	| sed -e "s;@prefix@;$(prefix:\=\\);g" \
	| sed -e "s;@imbindir@;$(prefix:\=\\)\\ImageMagick;g" \
	> tex4ht.env

mk4ht.pl: mk4ht
	-$(copy) mk4ht mk4ht.pl $(redir_stdout)

#tex4ht.bat: tex4ht.perl
#	pl2bat -s .perl tex4ht.perl

installdirs = $(texmf)\tex4ht\config $(prefix)\setupw32

!include <msvc/config.mak>
!include <msvc/install.mak>

install:: install-dirs install-exec install-data

install-exec:: $(programs)
	-@echo $(verbose) & ( \
		for %i in (ht httex htlatex httexi) do \
			$(copy) $(objdir)\runht.exe $(bindir)\%i.exe \
	) $(redir_stdout)
# we don't need it
	-@$(del) $(bindir)\runht.exe
	-@echo $(verbose) & ( \
		if not "$(perlscripts)"=="" ( \
			for %%s in ($(perlscripts)) do \
				$(copy) %%s $(texmf)\perl\tex4ht\%%s $(redir_stdout) \
				& $(perlcomp) $(perlcompflags) %%s >> $(win32perldir)\perlfiles.lst \
				& $(runperl) -f %%s $(bindir)\%%~ns.exe $(redir_stderr) \
		) \
	)

install-data:: tex4ht.env
#	xcopy .\texmf $(texmf) /r/i/e/d/k/y
#	$(copy) .\tex4ht.env $(texmf)\tex4ht\config
#	$(copy) tex4ht.imagick $(prefix)\setupw32
	$(copy) tex4ht.imagick $(texmf)\tex4ht\config
#	$(copy) tex4ht.netpbm $(prefix)\setupw32
	$(copy) tex4ht.netpbm $(texmf)\tex4ht\config

!include <msvc/clean.mak>

extraclean::
	-@$(del) tex4ht.env mk4ht.pl

!include <msvc/rdepend.mak>
!include "./depend.mak"

#
# Local Variables:
# mode: makefile
# End:
