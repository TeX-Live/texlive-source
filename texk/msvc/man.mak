################################################################################
#
# Makefile  : man, build man pages in nroff, html and dvi format
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <04/03/20 15:50:40 popineau>
#
################################################################################

#
# Only if manfiles or htmlmanfiles has been defined
#
!if ("$(manfiles)" != "" || "$(htmlmanfiles)" != "")

#
# Beware: these definitions will only override
# already present definitions in your environment
#
GROFF_FONT_PATH=.;$(gnushare:\=/)/groff/font
GROFF_TMAC_PATH=.;$(gnushare:\=/)/groff/tmac

!ifndef docdir 
docdir = $(texmf)\doc
!endif

!ifndef mandir
mandir = $(docdir)\man
!endif

psmandir = $(docdir)\man
dvimandir = $(docdir)\man
htmlmandir = $(docdir)\man

!ifndef mansedscript
mansedscript = $(win32seddir)\mansedscript
!endif

# The edited file always has extension .1; we change it when we install.
.SUFFIXES: .man .1 .3 .5 .8 .txt .ps .dvi .html
.man.1:
	$(silent)$(sed) -f $(mansedscript) $< >$@

.1.dvi:
#	$(DITROFF) -Tdvi -man $< >$@
#	echo $(GROFF_FONT_PATH)
	$(silent)$(troff) -e -t -man -Tdvi $< >$@

.1.ps:
#	$(DITROFF) -Tps -man $< >$@
	$(silent)$(troff) -e -t -man -Tps $< | $(grops) >$@

.1.txt:
#	$(DITROFF) -Tascii -man $< | col -b | expand >$@
	$(silent)$(troff) -e -t -man -Tascii $< | $(col) -b | $(expand) >$@

.3.dvi:
#	$(DITROFF) -Tdvi -man $< >$@
#	echo $(GROFF_FONT_PATH)
	$(silent)$(troff) -e -t -man -Tdvi $< >$@

.3.ps:
#	$(DITROFF) -Tps -man $< >$@
	$(silent)$(troff) -e -t -man -Tps $< >$@

.3.txt:
#	$(DITROFF) -Tascii -man $< | col -b | expand >$@
	$(silent)$(troff) -e -t -man -Tascii $< | $(col) -b | $(expand) >$@

.5.dvi:
#	$(DITROFF) -Tdvi -man $< >$@
#	echo $(GROFF_FONT_PATH)
	$(silent)$(troff) -e -t -man -Tdvi $< >$@

.5.ps:
#	$(DITROFF) -Tps -man $< >$@
	$(silent)$(troff) -e -t -man -Tps $< >$@

.5.txt:
#	$(DITROFF) -Tascii -man $< | col -b | expand >$@
	$(silent)$(troff) -e -t -man -Tascii $< | $(col) -b | $(expand) >$@

.8.dvi:
#	$(DITROFF) -Tdvi -man $< >$@
#	echo $(GROFF_FONT_PATH)
	$(silent)$(troff) -e -t -man -Tdvi $< >$@

.8.ps:
#	$(DITROFF) -Tps -man $< >$@
	$(silent)$(troff) -e -t -man -Tps $< >$@

.8.txt:
#	$(DITROFF) -Tascii -man $< | col -b | expand >$@
	$(silent)$(troff) -e -t -man -Tascii $< | $(col) -b | $(expand) >$@

.man.html:
	$(silent)$(sed) -f $(mansedscript) $< | $(rman) -f HTML -n $(<:.man=.1) | $(perl) $(win32perldir)/fix-rman.pl --title=$(<:.man=.1) > $@

.1.html:
	$(silent)$(rman) -f HTML -n $(<:.man=.1) $< | $(perl) $(win32perldir)/fix-rman.pl  > $@

.3.html:
	$(silent)$(rman) -f HTML -n $(<:.man=.3) $< | $(perl) $(win32perldir)/fix-rman.pl  > $@

.5.html:
	$(silent)$(rman) -f HTML -n $(<:.man=.5) $< | $(perl) $(win32perldir)/fix-rman.pl  > $@

.8.html:
	$(silent)$(rman) -f HTML -n $(<:.man=.8) $< | $(perl) $(win32perldir)/fix-rman.pl  > $@

# dvimanfiles = $(manfiles:.1=.dvi) $(manfiles:.3=.dvi) $(manfiles:.5=.dvi) $(manfiles:.8=.dvi) 
dvimanfiles = 
psmanfiles = $(manfiles:.1=.ps) $(manfiles:.3=.ps) $(manfiles:.5=.ps) $(manfiles:.8=.ps)
htmlmanfiles = $(manfiles:.1=.html) $(manfiles:.3=.html) $(manfiles:.5=.html) $(manfiles:.8=.html)
txtmanfiles = $(manfiles:.1=.txt) $(manfiles:.3=.txt) $(manfiles:.5=.txt) $(manfiles:.8=.txt)

$(manfiles): $(mansedscript)
$(htmlmanfiles): $(mansedscript)

man: $(manfiles) $(htmlmanfiles)

# We do not depend on the top-level Makefile since the top-level
# Makefile can change for reasons that do not affect the man pages.
# At present, all but VERSION should be unused.
$(win32seddir)\mansedscript.sed:
	@echo off <<$@
s/"//g
s/[ 	]*\/\*[^*]*\*\///g
<<KEEP

$(win32seddir)\mansedscript: $(win32seddir)\mansedscript.sed $(kpathseadir)\paths.h
	-@$(del) $(win32seddir)\mansedscript $(redir_stderr)
	-@echo $(verbose) & ( \
	for %f in ($(kpathseadir)/paths.h) do \
	  $(sed) -n -e "/^#define/s/#define[ 	][ 	]*\([A-Z_a-z][A-Z_a-z]*\)[ 	][ 	]*\(.*\)/s%@\1@%\2%/p" %f \
	  | $(sed) -f $(win32seddir)\mansedscript.sed >>$(win32seddir)\mansedscript.1 \
	) $(redir_stdout)
	@echo ""<<$(win32seddir)\mansedscript.2 $(redir_stdout)
s%@VERSION@%$(web2cmaj).$(web2cmin).$(web2cbeta)%
s%@BINDIR@%$(bindir)%
s%@INFODIR@%$(infodir)%
s%@TEXINPUTDIR@%$(texinputdir)%
s%@MFINPUTDIR@%$(mfinputdir)%
s%@MPINPUTDIR@%$(mpinputdir)%
s%@FONTDIR@%$(fontdir)%
s%@FMTDIR@%$(fmtdir)%
s%@BASEDIR@%$(basedir)%
s%@MEMDIR@%$(memdir)%
s%@TEXPOOLDIR@%$(texpooldir)%
s%@MFPOOLDIR@%$(mfpooldir)%
s%@MPPOOLDIR@%$(mppooldir)%
s%@FONTMAPDIR@%$(dvipsdir)%
s%@LOCALMODES@%$(localmodes)%
<<KEEP
	-@$(copy) $(win32seddir)\mansedscript.1 + $(win32seddir)\mansedscript.2 $(win32seddir)\mansedscript.3 $(redir_stdout)
	-$(sed) "s!$(prefix:\=\\)!c:\\Program Files\\TeXLive!g" < $(win32seddir)\mansedscript.3 > $(win32seddir)\mansedscript
	-@$(del) $(win32seddir)\mansedscript.1 $(win32seddir)\mansedscript.2 $(win32seddir)\mansedscript.3 $(redir_stdout)

!if !defined(MAKE_KPATHSEA)
$(kpathseadir)\paths.h:
	-@echo $(verbose) & (						\
		pushd $(kpathseadir) & $(make) paths.h & popd		\
	)
!endif

installdirs = $(installdirs) $(mandir) $(mandir)\man1 $(mandir)\man3 $(mandir)\man5 $(mandir)\man8 $(htmlmandir)

install:: install-man

install-man:: $(manfiles) $(htmlmanfiles)
	-@$(del) install-manpages.bat install-manpages.new $(redir_stderr)
	-@echo $(verbose) & for %i in ($(manfiles)) do					\
		echo $(copy) %i $(mandir)\man%~xi\%~nxi  >> install-manpages.bat &	\
		echo $(copy) %~ni.html $(mandir)\man%~xi\%~ni.html  >> install-manpages.bat	
	$(silent)$(sed) -e "1,$$s/man\.\([0-9]\)/man\1/g" < install-manpages.bat > install-manpages.new
	$(silent)$(copy) install-manpages.new install-manpages.bat $(redir_stdout)
	$(silent)$(del) install-manpages.new
	-@echo $(verbose) & install-manpages.bat $(redir_stdout)
	$(silent)$(del) install-manpages.bat

extraclean::
	-@echo $(verbose) & ( \
	echo Cleaning up extra man files in $(MAKEDIR) & \
	$(silent)$(del) mansedscript $(win32seddir)\mansedscript \
		mansedscript.1 mansedscript.2 $(redir_stderr) &		\
	for %i in ($(manfiles)) do			\
            $(del) %~ni.ps %~ni.html %~ni.pdf $(redir_stderr) \
	)
	-@echo $(verbose) & ( 				\
		 for %i in ($(manfiles)) do		\
	    		if exist %~ni.man $(del) %~ni.1 $(redir_stderr) \
	)

!else
#
# If no manpages have been declared, do nothing
#
install-man::

!endif

# End of man.mak
#  
# Local variables:
# page-delimiter: "^# \f"
# mode: Makefile
# End:
