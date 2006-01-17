################################################################################
#
# Makefile  : Aleph, Web2C win32.mak makefile fragment to build Aleph
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <04/03/20 16:42:18 popineau>
#
################################################################################

# Aleph itself.

aleph_build = rc2

# We build aleph
aleph = $(objdir)\aleph.exe
!ifdef TEX_DLL
aleph = $(aleph) $(objdir)\$(library_prefix)aleph.dll
!endif

# The C sources.
aleph_c = aleph.c
aleph_o = $(objdir)\aleph.obj $(objdir)\alephfirst.obj $(objdir)\alephbis.obj

# Generation of the web and ch files.

alephdir = .\alephdir

#
# e-Omega is build without the xml support
# so we can't reuse aleph.web and aleph.ch
# we need to build stripped down versions
#
aleph_files = \
#	$(alephdir)\omnode.ch \
	$(alephdir)\om16bit.ch \
	$(alephdir)\omstr.ch \
	$(alephdir)\omfont.ch \
	$(alephdir)\omchar.ch \
	$(alephdir)\omfi.ch \
	$(alephdir)\ompar.ch \
	$(alephdir)\omocp.ch \
	$(alephdir)\omfilter.ch \
	$(alephdir)\omtrans.ch \
	$(alephdir)\omdir.ch \
	$(alephdir)\eobase.ch \
	$(alephdir)\eofmt.ch \
	$(alephdir)\eomem-$(aleph_build).ch \
	$(alephdir)\eo16bit-$(aleph_build).ch \
	$(alephdir)\eoext.ch \
	$(alephdir)\eoeqtb.ch \
	$(alephdir)\eofix.ch \
	$(alephdir)\eocprt.ch \
	$(alephdir)\eonewdir-$(aleph_build).ch \
	$(alephdir)\eover-$(aleph_build).ch \
	$(alephdir)\eopage-$(aleph_build).ch \
	$(alephdir)\eochar-$(aleph_build).ch

aleph_changefiles = \
#	$(alephdir)\omega.ch0 	\
#	$(alephdir)\comnode.ch \
	$(alephdir)\com16bit-$(aleph_build).ch \
	$(alephdir)\comstr.ch \
	$(alephdir)\comfont.ch \
	$(alephdir)\comchar.ch \
	$(alephdir)\comfi.ch \
	$(alephdir)\compar.ch \
	$(alephdir)\comocp.ch \
	$(alephdir)\comfilter.ch \
	$(alephdir)\comtrans.ch \
	$(alephdir)\comdir.ch \
#	$(alephdir)\comxml.ch \
	$(alephdir)\comsrcspec.ch \
	$(alephdir)\ceostuff.ch \
	$(alephdir)\comw32.ch \
#	$(alephdir)\omega.ch1	\
#	$(alephdir)\eomega.ech	\
#	$(alephdir)\omega.ch2

# Linking
!ifdef TEX_DLL
$(ojbdir)\$(library_prefix)aleph.exp: $(objdir)\$(library_prefix)aleph.lib

$(objdir)\$(library_prefix)aleph.lib: $(aleph_o)
	$(archive) /DEF $(aleph_o)

$(objdir)\$(library_prefix)aleph.dll: $(aleph_o) $(objdir)\$(library_prefix)aleph.exp $(objdir)\aleph.res $(alephlibsdep) $(kpathsealib) $(proglib)
	$(link_dll) $(**) $(conlibs)

$(objdir)\aleph.exe: $(objdir)\win32main.obj $(objdir)\$(library_prefix)aleph.lib $(proglib)
	$(link) $(**) $(socketslib) $(conlibs)
!else
$(objdir)\aleph.exe: $(aleph_o) $(objdir)\win32main.obj $(kpathsealib) $(proglib)
	$(link) $(**) $(socketlibs) $(conlibs)
!endif

# The C files
$(aleph_c) alephcoerce.h alephd.h: aleph.p $(web2c_texmf)
	$(web2c) aleph
# Additional C files, just copy them.
alephfirst.c: alephdir\aleph.c
	-$(del) $@
	$(copy) $(srcdir)\alephdir\aleph.c $@
alephbis.c: alephdir\alephbis.c 
	-$(del) $@
	$(copy) $(srcdir)\alephdir\alephbis.c $@
alephmem.h: alephdir\alephmem.h
	-$(del) $@
	$(copy) $(srcdir)\alephdir\alephmem.h $@

# The Pascal file
aleph.p aleph.pool: $(objdir)\otangle.exe aleph.web aleph.ch
	$(objdir)\otangle aleph.web aleph.ch

# Generation of the web and ch files.
aleph.web: $(objdir)\tie.exe tex.web $(aleph_files)
	$(objdir)\tie.exe -m $@ tex.web $(aleph_files)

aleph.ch: $(objdir)\tie.exe aleph.web $(aleph_changefiles)
	$(objdir)\tie.exe -c $@ aleph.web $(aleph_changefiles)

# Check: right now all we do is build the format.
check: aleph-check
aleph-check: aleph aleph.afmt
# Cleaning up from building aleph
clean:: aleph-clean
aleph-clean:
	-@echo $(verbose) & ( \
		for %%i in ($(aleph_c) alephextra.c alephcoerce.h alephd.h \
			    alephfirst.c alephbis.c aleph.p aleph.pool     \
	                    aleph.web aleph.ch) do $(del) %%i $(redir_stderr) \
	)
# 
# Dumps.
# all_afmts = aleph.afmt $(afmts)

# dumps: afmts
# afmts: $(all_afmts)

# aleph.afmt: $(aleph)
# 	$(dumpenv) $(make) progname=aleph files="omega.tex" prereq-check
# 	$(dumpenv) .\$(objdir)\aleph --ini --progname=aleph --jobname=aleph "\\input omega.tex \\dump" < nul
# 
# elambda.afmt: $(aleph)
# 	$(dumpenv) $(make) progname=elambda files="lambda.tex" prereq-check
# 	$(dumpenv) .\$(objdir)\aleph --ini --progname=elambda --progname=elambda --jobname=elambda "\\input lambda.tex" < nul


# 
# Installation.
install-aleph: install-aleph-exec install-aleph-data
install-aleph-exec: install-aleph-links
# install-aleph-data: install-aleph-dumps
install-aleph-dumps: install-aleph-fmts

# The actual binary executables and pool files.
install-programs: install-aleph-programs
install-aleph-programs: $(aleph) $(bindir)
	-@echo $(verbose) & ( \
	  for %%p in ($(aleph)) do copy %%p $(bindir) $(redir_stdout) \
	)

# install-links: install-aleph-links
install-aleph-links: install-aleph-programs
# 	-@echo $(verbose) & ( \
# 	  pushd $(bindir) & \
# 	    $(del) inialeph.exe viraleph.exe & \
# 	    $(lnexe) .\aleph.exe .\inialeph.exe & \
# 	    $(lnexe) .\aleph.exe .\viraleph.exe & \
# 	  popd \
# 	) $(redir_stdout)
	-@echo $(verbose) & ( \
	  if not "$(afmts)"=="" \
	    for %%i in ($(afmts)) do \
              pushd $(bindir) & \
                $(del) .\%%~ni.exe & \
	  	$(lnexe) .\aleph.exe $(bindir)\%%~ni.exe & \
	      popd \
	) $(redir_stdout)

# install-fmts: install-aleph-fmts
install-aleph-fmts: afmts $(fmtdir)
	-@echo $(verbose) & ( \
	  if not "$(afmts)"=="" \
	    for %%f in ($(afmts)) do $(copy) %%f $(fmtdir)\%%f $(redir_stdout) \
	) $(redir_stdout)

# Auxiliary files.
install-data:: install-aleph-data
install-aleph-data: $(texpooldir)
	@$(copy) aleph.pool $(texpooldir)\aleph.pool $(redir_stdout)

# end of aleph.mak
#  
# Local variables:
# page-delimiter: "^# \f"
# mode: Makefile
# End:
