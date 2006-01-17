################################################################################
#
# Makefile  : pdftex, web2c win32.mak fragment to build pdfTeX
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <04/03/20 16:40:35 popineau>
#
################################################################################

# The libraries are not mentioned.  As the matter stands, a change in their
# number or how they are configured requires changes to the main distribution
# anyway.

Makefile: pdftexdir\pdftex.mk

pdftex_bin = $(objdir)\pdftex.exe $(objdir)\pdfetex.exe $(objdir)\ttf2afm.exe $(objdir)\pdftosrc.exe
# pdftex_exe = pdftex.exe pdfetex.exe ttf2afm.exe pdftosrc.exe
pdftex_pool = pdftex.pool pdfetex.pool
# linux_build_dir = $(HOME)\pdftex\build\linux\texk\web2c

# We build pdftex
pdftex = $(objdir)\pdftex.exe
!ifdef TEX_DLL
pdftex = $(pdftex) $(objdir)\$(library_prefix)pdftex.dll
!endif

# The C sources.
pdftex_c = pdftex.c
pdftex_o = $(objdir)\pdftex.obj

# Making pdftex
!ifdef TEX_DLL
$(objdir)\$(library_prefix)pdftex.exp: $(objdir)\$(library_prefix)pdftex.lib

$(objdir)\$(library_prefix)pdftex.lib: $(pdftex_o)
	$(archive) /DEF $(pdftex_o)

$(objdir)\$(library_prefix)pdftex.dll: $(pdftex_o) $(objdir)\$(library_prefix)pdftex.exp $(objdir)\pdftex.res $(pdftexlibs) $(kpathsealib) $(proglib)
	$(link_dll) $(**) $(socketlibs) $(conlibs)

$(objdir)\pdftex.exe: $(objdir)\win32main.obj $(objdir)\$(library_prefix)pdftex.lib $(kpathsealib) $(proglib)
	$(link) $(**) $(conlibs)
!else
$(objdir)\pdftex.exe: $(pdftex_o) $(objdir)\win32main.obj $(objdir)\pdftex.res $(pdftexlibs) $(kpathsealib) $(proglib)
	$(link) $(**) $(socketlibs) $(conlibs)
!endif

# C file dependencies.
$(pdftex_c) pdftexcoerce.h pdftexd.h: pdftex.p $(web2c_texmf)
	$(web2c) pdftex

# Tangling.
pdftex.p pdftex.pool: .\$(objdir)\tangle.exe pdftex.web pdftex.ch
	.\$(objdir)\tangle pdftex.web pdftex.ch

# Generation of the web and ch files.
pdftex.web: $(objdir)\tie.exe \
	tex.web \
        pdftexdir/pdftex.ch \
        pdftexdir/hz.ch \
        pdftexdir/misc.ch \
	# pdftexdir\pdftex.mak
	.\$(objdir)\tie -m pdftex.web tex.web \
	pdftexdir\pdftex.ch \
	$(srcdir)/pdftexdir/hz.ch  \
	$(srcdir)/pdftexdir/misc.ch

pdftex.ch: $(objdir)\tie.exe pdftex.web	\
	   pdftexdir\tex.ch0		\
	   tex.ch			\
	   tex-supp-w32.ch		\
	   pdftexdir\tex.ch1		\
	   pdftexdir\tex.pch		\
	   pdftexdir\tex.ch2
#	   pdftexdir\pdftex.mak
	.\$(objdir)\tie -c pdftex.ch    \
	   pdftex.web 			\
	   $(srcdir)\pdftexdir\tex.ch0	\
	   tex.ch			\
	   tex-supp-w32.ch		\
	   $(srcdir)\pdftexdir\tex.ch1	\
	   $(srcdir)\pdftexdir\tex.pch	\
	   $(srcdir)\pdftexdir\tex.ch2

# Tests...
check: pdftex-check
pdftex-check: pdftex pdftex.fmt

# Cleaning up.
clean:: pdftex-clean
pdftex-clean:
#	$(LIBTOOL) --mode=clean $(RM) pdftex
	-@echo $(verbose) & ( \
		for %%i in ($(pdftex_o) $(pdftex_c) pdftexextra.c pdftexcoerce.h \
			    pdftexd.h pdftex.p pdftex.pool pdftex.web pdftex.ch \
                            pdftex.fmt pdftex.log) do $(del) %%i $(redir_stderr) \
	)

# Dumps.
# all_pdffmts = pdftex.fmt $(pdffmts)

# dumps: pdffmts
# pdffmts: $(all_pdffmts)
# pdftex.fmt: $(pdftex)
# 	$(dumpenv) $(make) progname=pdftex files="plain.tex cmr10.tfm" prereq-check
# 	$(dumpenv) .\$(objdir)\pdftex --progname=pdftex --jobname=pdftex --ini "\pdfoutput=1 \input plain \dump" <nul
# 
# pdfolatex.fmt: $(pdftex)
# 	$(dumpenv) $(make) progname=pdfolatex files="latex.ltx" prereq-check
# 	$(dumpenv) .\$(objdir)\pdftex --progname=pdfolatex --jobname=pdfolatex --ini "\pdfoutput=1 \input latex.ltx" <nul
# 
# pdflatex.fmt: $(pdftex)
# 	$(dumpenv) $(make) progname=pdflatex files="latex.ltx" prereq-check
# 	$(dumpenv) .\$(objdir)\pdftex --progname=pdflatex --jobname=pdflatex --ini "\pdfoutput=1 \input latex.ltx" <nul
# 
# 
# Installation.
install-pdftex: install-pdftex-exec install-pdftex-data
install-pdftex-exec: install-pdftex-links
# install-pdftex-data: install-pdftex-dumps
install-pdftex-dumps: install-pdftex-fmts

# The actual binary executables and pool files.
install-programs: install-pdftex-programs
install-pdftex-programs: $(pdftex) $(bindir)
	-@echo $(verbose) & ( \
	  for %%p in ($(pdftex)) do $(copy) %%p $(bindir) \
	) $(redir_stdout)

# install-links: install-pdftex-links
install-pdftex-links: install-pdftex-programs
# 	-@echo $(verbose) & ( \
# 	  pushd $(bindir) & \
# 	    $(del) .\pdfinitex.exe .\pdfvirtex.exe & \
# 	    $(lnexe) .\pdftex.exe $(bindir)\pdfinitex.exe & \
# 	    $(lnexe) .\pdftex.exe $(bindir)\pdfvirtex.exe & \
# 	 popd \
# 	) $(redir_stdout)
	-@echo $(verbose) & ( \
	  if not "$(pdffmts)"=="" \
	    for %%i in ($(pdffmts)) do \
              pushd $(bindir) & \
                $(del) .\%%~ni.exe & \
	        $(lnexe) .\pdftex.exe $(bindir)\%%~ni.exe & \
	      popd \
	) $(redir_stdout)

# install-fmts: install-pdftex-fmts
install-pdftex-fmts: pdffmts $(fmtdir)
	-@echo $(verbose) & ( \
	  for %%f in ($(all_pdffmts)) \
	    do $(copy) %%f $(fmtdir)\%%f \
	) $(redir_stdout)

# Auxiliary files.
install-data:: install-pdftex-data
install-pdftex-data: $(texpooldir)
	@$(copy) pdftex.pool $(texpooldir)\pdftex.pool $(redir_stdout)

# 
# ttf2afm
ttf2afm = $(objdir)\ttf2afm.exe

$(objdir)\ttf2afm.exe: $(objdir)\ttf2afm.obj
	$(link) $(**) $(kpathsealib) $(proglib) $(conlibs)

$(objdir)\ttf2afm.obj: pdftexdir\ttf2afm.c pdftexdir\macnames.c
	$(compile) -I.\pdftexdir pdftexdir\ttf2afm.c
check: ttf2afm-check
ttf2afm-check: $(objdir)\ttf2afm.exe
clean:: ttf2afm-clean
ttf2afm-clean:
#	$(LIBTOOL) --mode=clean $(RM) ttf2afm
	-@echo $(verbose) & ( \
		for %%i in ($(objdir)\ttf2afm.obj $(objdir)\macnames.obj \
			    ttf2afm.c macnames.c) do $(del) %%i $(redir_stderr) \
	)
# 
# pdftosrc
pdftosrc = $(objdir)\pdftosrc.exe

$(objdir)\pdftosrc.exe: $(objdir)\pdftosrc.obj $(xpdflib)
	$(link) $(objdir)\pdftosrc.obj $(xpdflib) $(kpathsealib) $(proglib) $(conlibs)

$(objdir)\pdftosrc.obj: pdftexdir\pdftosrc.cc
	$(compile) /Tp $(**)
check: pdftosrc-check
pdftosrc-check: $(objdir)\pdftosrc.exe
clean:: pdftosrc-clean
pdftosrc-clean:
#	$(LIBTOOL) --mode=clean $(RM) pdftosrc
	-@$(del) $(objdir)\pdftosrc.obj $(redir_stderr)

# end of pdftex.mak
#  
# Local variables:
# page-delimiter: "^# \f"
# mode: Makefile
# End:
