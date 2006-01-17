################################################################################
#
# Makefile  : Omega, web2c win32.mak makefile fragment to build Omega
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <04/03/20 16:43:02 popineau>
#
################################################################################

# e-Omega itself.

# We build eomega
eomega = $(objdir)\eomega.exe
!ifdef TEX_DLL
eomega = $(eomega) $(objdir)\$(library_prefix)eomega.dll
!endif

# The C sources.
eomega_c = eomega.c
eomega_o = $(objdir)\eomega.obj $(objdir)\eomegafirst.obj $(objdir)\eomegabis.obj

# Generation of the web and ch files.

omegadir = .\omegadir-1.15
eomegadir = .\eomegadir

#
# e-Omega is build without the xml support
# so we can't reuse omega.web and omega.ch
# we need to build stripped down versions
#
eomega_files = \
#	$(omegadir)\omnode.ch \
	$(omegadir)\om16bit.ch \
	$(omegadir)\omstr.ch \
	$(omegadir)\omfont.ch \
	$(omegadir)\omchar.ch \
	$(omegadir)\omfi.ch \
	$(omegadir)\ompar.ch \
	$(omegadir)\omocp.ch \
	$(omegadir)\omfilter.ch \
	$(omegadir)\omtrans.ch \
	$(omegadir)\omdir.ch \
#	$(omegadir)\omxml.ch # We don't want this one, really, do we?
	$(eomegadir)\eocprt.ch \
	$(eomegadir)\eobase.ch \
	$(eomegadir)\eomem-rc1.ch \
	$(eomegadir)\eo16bit-rc1.ch \
	$(eomegadir)\eofix.ch \
	$(eomegadir)\eofix2.ch \
	$(eomegadir)\eoeqtb.ch \
	$(eomegadir)\eonewdir-rc1.ch \
	$(eomegadir)\eover-rc1.ch

eomega_changefiles = \
	$(eomegadir)\omega.ch0 	\
#	$(omegadir)\comnode.ch \
	$(omegadir)\com16bit.ch \
	$(omegadir)\comstr.ch \
	$(omegadir)\comfont.ch \
	$(omegadir)\comchar.ch \
	$(omegadir)\comfi.ch \
	$(omegadir)\compar.ch \
	$(omegadir)\comocp.ch \
	$(omegadir)\comfilter.ch \
	$(omegadir)\comtrans.ch \
	$(omegadir)\comdir.ch \
#	$(omegadir)\comxml.ch \
	$(omegadir)\comsrcspec.ch \
	$(omegadir)\comw32.ch \
	$(eomegadir)\omega.ch1	\
	$(eomegadir)\eomega.ech	\
	$(eomegadir)\omega.ch2

# Linking
!ifdef TEX_DLL
$(ojbdir)\$(library_prefix)eomega.exp: $(objdir)\$(library_prefix)eomega.lib

$(objdir)\$(library_prefix)eomega.lib: $(eomega_o)
	$(archive) /DEF $(eomega_o)

$(objdir)\$(library_prefix)eomega.dll: $(eomega_o) $(objdir)\$(library_prefix)eomega.exp $(objdir)\eomega.res $(omegalibsdep) $(kpathsealib) $(proglib)
	$(link_dll) $(**) $(conlibs)

$(objdir)\eomega.exe: $(objdir)\win32main.obj $(objdir)\$(library_prefix)eomega.lib $(proglib)
	$(link) $(**) $(socketslib) $(conlibs)
!else
$(objdir)\eomega.exe: $(eomega_o) $(objdir)\win32main.obj $(kpathsealib) $(proglib)
	$(link) $(**) $(socketlibs) $(conlibs)
!endif

# The C files
$(eomega_c) eomegacoerce.h eomegad.h: eomega.p $(web2c_texmf)
	$(web2c) eomega
# Additional C files, just copy them.
eomegafirst.c: omegadir\omega.c
	-$(del) $@
	$(copy) $(srcdir)\omegadir\omega.c $@
eomegabis.c: omegadir\omegabis.c 
	-$(del) $@
	$(copy) $(srcdir)\omegadir\omegabis.c $@
eomegamem.h: omegadir\omegamem.h
	-$(del) $@
	$(copy) $(srcdir)\omegadir\omegamem.h $@

# The Pascal file
eomega.p eomega.pool: $(objdir)\otangle.exe eomega.web eomega.ch
	$(objdir)\otangle eomega.web eomega.ch

# Generation of the web and ch files.
eomega.web: $(objdir)\tie.exe tex.web $(eomega_files)
	$(objdir)\tie.exe -m $@ tex.web $(eomega_files)

eomega.ch: $(objdir)\tie.exe eomega.web $(eomega_changefiles)
	$(objdir)\tie.exe -c $@ eomega.web $(eomega_changefiles)

# Check: right now all we do is build the format.
check: eomega-check
eomega-check: eomega eomega.eoft
# Cleaning up from building omega
clean:: eomega-clean
eomega-clean:
	-@echo $(verbose) & ( \
		for %%i in ($(eomega_c) eomegaextra.c eomegacoerce.h eomegad.h \
			    eomegafirst.c eomegabis.c eomegamem.h \
			    eomega.p eomega.pool eomega.web eomega.ch) do $(del) %%i $(redir_stderr) \
	)
# 
# Dumps.
# all_eofmts = eomega.eoft $(eofmts)
# 
# dumps: eofmts
# eofmts: $(all_eofmts)
# 
# eomega.eoft: $(eomega)
# 	$(dumpenv) $(make) progname=eomega files="omega.tex" prereq-check
# 	$(dumpenv) .\$(objdir)\eomega --ini --progname=eomega --jobname=eomega "\\input omega.tex \\dump" < nul
# 
# elambda.eoft: $(eomega)
# 	$(dumpenv) $(make) progname=elambda files="lambda.tex" prereq-check
# 	$(dumpenv) .\$(objdir)\eomega --ini --progname=elambda --progname=elambda --jobname=elambda "\\input lambda.tex" < nul


# 
# Installation.
install-eomega: install-eomega-exec install-eomega-data
install-eomega-exec: install-eomega-links
# install-eomega-data: install-eomega-dumps
install-eomega-dumps: install-eomega-fmts

# The actual binary executables and pool files.
install-programs: install-eomega-programs
install-eomega-programs: $(eomega) $(bindir)
	-@echo $(verbose) & ( \
	  for %%p in ($(eomega)) do copy %%p $(bindir) $(redir_stdout) \
	)

install-links: install-eomega-links
install-eomega-links: install-eomega-programs
# 	-@echo $(verbose) & ( \
# 	  pushd $(bindir) & \
# 	    $(del) inieomega.exe vireomega.exe & \
# 	    $(lnexe) .\eomega.exe .\inieomega.exe & \
# 	    $(lnexe) .\eomega.exe .\vireomega.exe & \
# 	  popd \
# 	) $(redir_stdout)
	-@echo $(verbose) & ( \
	  if not "$(eofmts)"=="" \
	    for %%i in ($(eofmts)) do \
              pushd $(bindir) & \
                $(del) .\%%~ni.exe & \
	  	$(lnexe) .\eomega.exe $(bindir)\%%~ni.exe & \
	      popd \
	) $(redir_stdout)

install-fmts: install-eomega-fmts
install-eomega-fmts: eofmts $(fmtdir)
	-@echo $(verbose) & ( \
	  if not "$(eofmts)"=="" \
	    for %%f in ($(eofmts)) do $(copy) %%f $(fmtdir)\%%f $(redir_stdout) \
	) $(redir_stdout)

# Auxiliary files.
install-data:: install-eomega-data
install-eomega-data: $(texpooldir)
	@$(copy) eomega.pool $(texpooldir)\eomega.pool $(redir_stdout)

# end of eomega.mak
#  
# Local variables:
# page-delimiter: "^# \f"
# mode: Makefile
# End:
