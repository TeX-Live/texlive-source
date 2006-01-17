################################################################################
#
# Makefile  : pdfeTeX, web2c win32.mak makefile fragment to build pdfe-TeX
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <04/03/20 17:02:03 popineau>
#
################################################################################

Makefile: $(srcdir)\pdfetexdir\pdfetex.mak

# We build pdfetex
pdfetex = $(objdir)\pdfetex.exe
!ifdef TEX_DLL
pdfetex = $(pdfetex) $(objdir)\$(library_prefix)pdfetex.dll
!endif

# The C sources.
pdfetex_c = pdfetex.c
pdfetex_o = $(objdir)\pdfetex.obj

!ifdef TEX_DLL
$(objdir)\$(library_prefix)pdfetex.exp: $(objdir)\$(library_prefix)pdfetex.lib

$(objdir)\$(library_prefix)pdfetex.lib: $(pdfetex_o)
	$(archive) /DEF $(pdfetex_o)

$(objdir)\$(library_prefix)pdfetex.dll: $(pdfetex_o) $(objdir)\$(library_prefix)pdfetex.exp $(objdir)\pdfetex.res $(pdftexlibs) $(kpathsealib) $(proglib)
	$(link_dll) $(**) $(socketlibs) $(conlibs)

$(objdir)\pdfetex.exe: $(objdir)\win32main.obj $(objdir)\$(library_prefix)pdfetex.lib $(proglib)
	$(link) $(**) $(conlibs)
!else
$(objdir)\pdfetex.exe: $(pdfetex_o) $(objdir)\win32main.obj $(objdir)\pdfetex.res $(pdftexlibs) $(kpathsealib) $(proglib)
	$(link) $(**) $(socketlibs) $(conlibs)
!endif

pdfetex: $(pdfetex)

# C file dependencies.
$(pdfetex_c) pdfetexcoerce.h pdfetexd.h: pdfetex.p $(web2c_texmf)
	$(web2c) pdfetex

# Tangling
pdfetex.p pdfetex.pool: $(objdir)\tangle.exe pdfetex.web pdfetex.ch
	.\$(objdir)\tangle pdfetex.web pdfetex.ch

pdfetex_files = \
	 etexdir\etex.ch0 \
	 etexdir\etex.ch \
	 etexdir\etex.fix \
	 etexdir\etex.ch1 \
         pdfetexdir\pdfetex.ch1 \
         pdftexdir\pdftex.ch \
         pdftexdir/hz.ch \
         pdftexdir/misc.ch \
         pdfetexdir\pdfetex.ch2

pdfetex_changefiles = \
	    pdfetexdir\tex.ch0 \
	    tex.ch \
	    tex-supp-w32.ch \
	    etexdir\tex.ch1 \
	    etexdir\tex.ech \
	    etexdir\tex.ch2 \
	    pdfetexdir\tex.ch1 \
	    pdftexdir\tex.pch \
	    pdfetexdir\tex.ch2 \
#	    pdfetexdir\tex.ch3

# Generation of the web and ch files.
pdfetex.web: $(objdir)\tie.exe tex.web $(pdfetex_files) \
	pdfetexdir\pdfetex.h pdfetexdir\pdfetex.defines # pdfetexdir\pdfetex.mak
	.\$(objdir)\tie -m pdfetex.web tex.web $(pdfetex_files)

pdfetex.ch: $(objdir)\tie.exe pdfetex.web $(pdfetex_changefiles) # pdfetexdir/pdfetex.mak
	.\$(objdir)\tie -c pdfetex.ch pdfetex.web $(pdfetex_changefiles)

pdfetexdir\pdfetex.h: pdftexdir\pdftex.h
	-@$(del) $(@) $(redir_stderr)
	$(copy) $(**) $(@)

pdfetexdir\pdfetex.defines: pdftexdir\pdftex.defines
	-@$(del) $(@) $(redir_stderr)
	$(copy) $(**) $(@)

check: pdfetex-check
pdfetex-check: pdfetex pdfetex.efmt

clean:: pdfetex-clean
pdfetex-clean:
#	$(LIBTOOL) --mode=clean $(RM) pdfetex
	-@echo $(verbose) & ( \
		for %%i in ($(pdfetex_o) $(pdfetex_c) pdfetexextra.c pdfetexcoerce.h \
			    pdfetexd.h pdfetex.p pdfetex.pool pdfetex.web pdfetex.ch \
			    pdfetex.efmt pdfetex.log) do $(del) %%i $(redir_stderr) \
	)

# Dumps
# all_pdfefmts = pdfetex.efmt $(pdfefmts)
# pdfefmts: $(all_pdfefmts)

# pdfetex.efmt: $(pdfetex)
# 	$(dumpenv) $(make) progname=pdfetex files="etex.src plain.tex cmr10.tfm" prereq-check
# 	$(dumpenv) ./pdfetex --progname=pdfetex --jobname=pdfetex --ini "*\pdfoutput=1\input etex.src \dump" <nul
# 
# pdfelatex.efmt: $(pdfetex)
# 	$(dumpenv) $(make) progname=pdfelatex files="latex.ltx" prereq-check
# 	$(dumpenv) ./pdfetex --progname=pdfelatex --jobname=pdfelatex --ini "*\pdfoutput=1\input latex.ltx" <nul
# 
# pdflatex.efmt: $(pdfetex)
# 	$(dumpenv) $(make) progname=pdflatex files="latex.ltx" prereq-check
# 	$(dumpenv) ./pdfetex --progname=pdflatex --jobname=pdflatex --ini "*\pdfoutput=1\input latex.ltx" <nul

# 
# Installation.
install-pdfetex: install-pdfetex-exec install-pdfetex-data
install-pdfetex-exec: install-pdfetex-links
# install-pdfetex-data: install-pdfetex-dumps
install-pdfetex-dumps: install-pdfetex-fmts

# The actual binary executables and pool files.
install-programs: install-pdfetex-programs
install-pdfetex-programs: $(pdfetex) $(bindir)
	-@echo $(verbose) & ( \
	  for %%p in ($(pdfetex)) do $(copy) %%p $(bindir) \
	) $(redir_stdout)

# install-links: install-pdfetex-links
install-pdfetex-links: install-pdfetex-programs
# 	-@echo $(verbose) & ( \
# 	  pushd $(bindir) & \
# 	    $(del) .\pdfeinitex.exe .\pdfevirtex.exe & \
# 	    $(lnexe) .\pdfetex.exe $(bindir)\pdfeinitex.exe & \
# 	    $(lnexe) .\pdfetex.exe $(bindir)\pdfevirtex.exe & \
# 	  popd \
# 	) $(redir_stdout)
	-@echo $(verbose) & ( \
	  if not "$(pdfefmts)"=="" \
	    for %%i in ($(pdfefmts)) do \
              pushd $(bindir) & \
                $(del) .\%%~ni.exe & \
	        $(lnexe) .\pdfetex.exe $(bindir)\%%~ni.exe & \
	      popd \
	) $(redir_stdout)

# install-fmts: install-pdfetex-fmts
install-pdfetex-fmts: pdfefmts $(fmtdir)
	-@echo $(verbose) & ( \
	  for %%f in ($(all_pdfefmts)) \
	    do $(copy) %%f $(fmtdir)\%%f \
	) $(redir_stdout)

# Auxiliary files.
install-data:: install-pdfetex-data
install-pdfetex-data: $(texpooldir)
	@$(copy) pdfetex.pool $(texpooldir)\pdfetex.pool $(redir_stdout)

# end of pdfetex.mak
#  
# Local variables:
# page-delimiter: "^# \f"
# mode: Makefile
# End:
