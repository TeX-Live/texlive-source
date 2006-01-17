################################################################################
#
# Makefile  : Omega, web2c win32.mak makefile fragment to build Omega
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <04/03/15 12:23:15 popineau>
#
################################################################################

Makefile: $(win32make_srcdir)/omega.mak

omegadir = omegadir-1.15

omegafonts_programs = omegafonts\$(objdir)\omfonts.exe
otps_programs = otps\$(objdir)\otp2ocp.exe otps\$(objdir)\outocp.exe
omegafonts = omegafonts
otps = otps

odvicopy = $(objdir)\odvicopy.exe
odvitype = $(objdir)\odvitype.exe
otangle = $(objdir)\otangle.exe
omegaware_c = odvicopy.c odvitype.c otangle.c
omegaware_programs = $(otangle) $(odvicopy) $(odvitype)

omega_programs = $(otangle) $(odvicopy) $(odvitype) \
                 $(otps_programs) $(omegafonts_programs)

# Omegaware

$(objdir)\odvicopy.exe: $(objdir)\odvicopy.obj $(kpathsealib) $(proglib)
	$(link) $(**) $(conlibs)

odvicopy.c odvicopy.h: $(web2c_common) $(web2c_programs) odvicopy.p
	$(web2c) odvicopy
odvicopy.p: omegaware\odvicopy.web omegaware\odvicopy.ch
	.\$(objdir)\tangle omegaware\odvicopy.web omegaware\odvicopy.ch
check: odvicopy-check
odvicopy-check: $(objdir)\odvicopy.exe
#	./odvicopy $(srcdir)/tests/story tests/xstory.dvi
## Redirect stderr so the terminal output will end up in the log file.
#	(TFMFONTS=$(srcdir)/tests: ./odvicopy <$(srcdir)/tests/pplr.dvi \
#	  >tests/xpplr.dvi) 2>&1
clean:: odvicopy-clean
odvicopy-clean:
#	$(LIBTOOL) --mode=clean $(RM) odvicopy
	-@echo $(verbose) & ( \
		for %%i in ($(objdir)\odvicopy.obj odvicopy.c odvicopy.h odvicopy.p) do $(del) %%i $(redir_stderr) \
	)
#	rm -f tests/xstory.dvi tests/xpplr.dvi

$(objdir)\odvitype.exe: $(objdir)\odvitype.obj $(kpathsealib) $(proglib)
	$(link) $(**) $(conlibs)

odvitype.c odvitype.h: $(web2c_common) $(web2c_programs) odvitype.p
	$(web2c) odvitype

odvitype.p: $(objdir)\tangle.exe omegaware\odvitype.web omegaware\odvitype.ch
	$(objdir)\tangle.exe omegaware\odvitype.web omegaware\odvitype.ch
check: odvitype-check
odvitype-check: $(objdir)\odvitype.exe
#	./odvitype -show-opcodes $(srcdir)/tests/story >tests/xstory.dvityp
#	./odvitype --p=\*.\*.2 $(srcdir)/tests/pagenum.dvi >tests/xpagenum.typ
clean:: odvitype-clean
odvitype-clean:
#	$(LIBTOOL) --mode=clean $(RM) odvitype
	-@echo $(verbose) & ( \
		for %%i in ($(objdir)\odvitype.obj odvitype.c odvitype.h odvitype.p) do $(del) %%i $(redir_stderr) \
	)
#	-$(del) tests/xstory.dvityp tests/xpagenum.typ

$(objdir)\otangle.exe: $(objdir)\otangle.obj $(objdir)\otangle.res $(kpathsealib) $(proglib)
	$(link) $(**) $(conlibs)
otangle.c otangle.h: $(web2c_common) $(web2c_programs) otangle.p
	$(web2c) otangle
otangle.p: $(objdir)\tangle.exe omegaware\otangle.web omegaware\otangle.ch
	$(objdir)\tangle omegaware\otangle.web omegaware\otangle.ch
check: otangle-check
otangle-check: otangle
#	otangling omega is a good check already
clean:: otangle-clean
otangle-clean:
#	$(LIBTOOL) --mode=clean $(RM) otangle
	-@echo $(verbose) & ( \
		for %%i in ($(objdir)\otangle.obj otangle.c otangle.h otangle.p) do $(del) %%i $(redir_stderr) \
	)
# 
# Omega itself.

# We build omega
omega = $(objdir)\omega.exe
!ifdef TEX_DLL
omega = $(omega) $(objdir)\$(library_prefix)omega.dll
!endif

omega_programs = $(omega) $(omega_programs)

# The C sources.
omega_c = omega.c
omega_o = $(objdir)\omega.obj $(objdir)\omegafirst.obj $(objdir)\omegabis.obj

# Linking
!ifdef TEX_DLL
$(ojbdir)\$(library_prefix)omega.exp: $(objdir)\$(library_prefix)omega.lib

$(objdir)\$(library_prefix)omega.lib: $(omega_o)
	$(archive) /DEF $(omega_o)

$(objdir)\$(library_prefix)omega.dll: $(omega_o) $(objdir)\$(library_prefix)omega.exp $(objdir)\omega.res $(omegalibsdep) $(kpathsealib) $(proglib)
	$(link_dll) $(**) $(conlibs)

$(objdir)\omega.exe: $(objdir)\win32main.obj $(objdir)\$(library_prefix)omega.lib $(proglib)
	$(link) $(**) $(socketslib) $(conlibs)
!else
$(objdir)\omega.exe: $(omega_o) $(objdir)\win32main.obj $(kpathsealib) $(proglib)
	$(link) $(**) $(socketlibs) $(conlibs)
!endif


# The C files
$(omega_c) omegacoerce.h omegad.h: omega.p $(web2c_texmf)
	$(web2c) omega
# Additional C files, just copy them.
omegafirst.c: $(omegadir)\omega.c
	-$(del) $@
	@$(copy) $(srcdir)\$(omegadir)\omega.c $@ $(redir_stdout)
omegabis.c: $(omegadir)\omegabis.c 
	-$(del) $@
	@$(copy) $(srcdir)\$(omegadir)\omegabis.c $@ $(redir_stdout)
omegamem.h: $(omegadir)\omegamem.h
	-$(del) $@
	@$(copy) $(srcdir)\$(omegadir)\omegamem.h $@ $(redir_stdout)

# The Pascal file
omega.p omega.pool: $(objdir)\otangle.exe omega.web omega.ch
	$(objdir)\otangle omega.web omega.ch

# Generation of the web and ch files.
omega_files = \
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
#	$(omegadir)\omxml.ch

omega_changefiles = \
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
	$(omegadir)\comw32.ch

omega.web: .\$(objdir)\tie.exe tex.web $(omega_files) # $(omegadir)\omega.mak
	.\$(objdir)\tie -m omega.web tex.web $(omega_files)

omega.ch: .\$(objdir)\tie.exe omega.web $(omega_changefiles) # $(omegadir)\omega.mak
	.\$(objdir)\tie -c omega.ch omega.web $(omega_changefiles)

# Check: right now all we do is build the format.
check: omega-check
omega-check: omega omega.oft
# Cleaning up from building omega
clean:: omega-clean
omega-clean:
#	$(LIBTOOL) --mode=clean $(RM) omega
	-@echo $(verbose) & ( \
		for %%i in ($(omega_o) $(omega_c) omegaextra.c omegacoerce.h omegad.h \
			    omegafirst.c omegabis.c omegamem.h \
                            omega.p omega.pool omega.web omega.ch) do $(del) %%i $(redir_stderr) \
	)
# 
# Dumps.
all_ofmts = omega.oft $(ofmts)

dumps: ofmts
ofmts: $(all_ofmts)

omega.oft: $(omega)
	$(dumpenv) $(make) progname=omega files="omega.tex" prereq-check
	$(dumpenv) .\$(objdir)\omega.exe --progname=omega --jobname=omega --ini "\input omega.tex \dump" < nul

lambda.oft: $(omega)
	$(dumpenv) $(make) progname=lambda files="lambda.tex" prereq-check
	$(dumpenv) .\$(objdir)\omega.exe --progname=lambda --jobname=lambda --ini "\input lambda.tex" < nul

# 
# Some additional programs for Omega: the programs themselves are named
# in the variable otps_programs, defined above.
otps\$(objdir)\otp2ocp.exe otps\$(objdir)\outocp.exe:
	-@echo $(verbose) & ( \
		pushd otps & $(make) all & popd \
	)
omegafonts\$(objdir)\omfonts.exe:
	-@echo $(verbose) & ( \
		pushd omegafonts & $(make) all & popd \
	)
# 
# Installation.
install-omega: install-omega-exec install-omega-data
install-omega-exec: install-omega-links
# install-omega-data: install-omega-dumps
install-omega-dumps: install-omega-fmts

# The actual binary executables and pool files.
install-programs: install-omega-programs
install-omega-programs: $(omega_programs) $(bindir)
	-@echo $(verbose) & ( \
	  for %p in ($(eomega)) do copy %%p $(bindir) \
	)
	-@echo $(verbose) & ( \
	  pushd otps & $(make) install-programs & popd \
	)
	-@echo $(verbose) & ( \
	  pushd omegafonts & $(make) install-programs & popd \
	)

install-links: install-omega-links
install-omega-links: install-omega-programs
	-@echo $(verbose) & ( \
	  pushd omegafonts & $(make) install-links & popd \
	)
#	-@echo $(verbose) & ( \
#	  pushd $(bindir) & \
#	    $(del) .\iniomega.exe .\viromega.exe & \
#	    $(lnexe) .\omega.exe $(bindir)\iniomega.exe & \
#	    $(lnexe) .\omega.exe $(bindir)\viromega.exe & \
#	  popd \
#	) $(redir_stdout)
	-@echo $(verbose) & ( \
	  if not "$(ofmts)"=="" \
	    for %%i in ($(ofmts)) do \
              pushd $(bindir) & \
                $(del) .\%%~ni.exe & \
	        $(lnexe) .\omega.exe $(bindir)\%%~ni.exe & \
	      popd \
	) $(redir_stdout)

# Always do plain.*, so examples from the TeXbook (etc.) will work.
install-fmts: install-omega-fmts
install-omega-fmts: ofmts $(fmtdir)
	-@echo $(verbose) & ( \
	if not "$(all_ofmts)"=="" \
	  for %%f in ($(all_ofmts)) do \
	    $(copy) %%f $(fmtdir)\%%f $(redir_stdout) \
	)

# Auxiliary files.
install-data:: install-omega-data
install-omega-data: $(texpooldir)
	@$(copy) omega.pool $(texpooldir)\omega.pool $(redir_stdout) $(redir_stdout)

# end of omega.mak
#  
# Local variables:
# page-delimiter: "^# \f"
# mode: Makefile
# End:
