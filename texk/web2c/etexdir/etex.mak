################################################################################
#
# Makefile  : eTeX, web2c win32.mak makefile fragment to build e-TeX
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <04/03/20 16:41:06 popineau>
#
################################################################################

etexver = 

# We build etex.
etex = $(objdir)\etex.exe
!ifdef TEX_DLL
etex = $(etex) $(objdir)\$(library_prefix)etex.dll
!endif

# The C sources.
etex_c = etex.c
etex_o = $(objdir)\etex.obj

!ifdef TEX_DLL
$(objdir)\$(library_prefix)etex.exp: $(objdir)\$(library_prefix)etex.lib

$(objdir)\$(library_prefix)etex.lib: $(etex_o)
	$(archive) /DEF $(etex_o)

$(objdir)\$(library_prefix)etex.dll: $(etex_o) $(objdir)\$(library_prefix)etex.exp $(objdir)\etex.res $(kpathsealib) $(proglib)
	$(link_dll) $(**) $(socketlibs) $(conlibs)

$(objdir)\etex.exe: $(objdir)\win32main.obj $(objdir)\$(library_prefix)etex.lib $(proglib)
	$(link) $(**) $(conlibs)
!else
$(objdir)\etex.exe: $(etex_o) $(objdir)\win32main.obj $(objdir)\etex.res $(kpathsealib) $(proglib)
	$(link) $(**) $(conlibs)
!endif

# C file dependencies
$(etex_c) etexcoerce.h etexd.h: etex.p $(web2c_texmf)
	$(web2c) etex

# Tangling
etex.p etex.pool: etex.web etex.ch
	.\$(objdir)\tangle etex.web etex.ch

# Generation of the web and ch file.
etex.web: $(objdir)\tie.exe tex.web etexdir/etex.ch0 etexdir/etex.ch etexdir/etex.fix etexdir/etex.ch1
	.\$(objdir)\tie -m etex.web		\
		           tex.web	\
			   etexdir/etex.ch0 \
			   etexdir/etex.ch \
			   etexdir/etex.fix \
		           etexdir/etex.ch1

etex.ch: $(objdir)\tie.exe etex.web etexdir/tex.ch0 tex.ch tex-supp-w32.ch etexdir/tex.ch1 etexdir/tex.ech etexdir/tex.ch2
	.\$(objdir)\tie -c etex.ch etex.web			\
			   etexdir/tex.ch0 	\
			   tex.ch	 		\
			   tex-supp-w32.ch	 		\
			   etexdir/tex.ch1 	\
			   etexdir/tex.ech	\
			   etexdir/tex.ch2

# Tests...
check: etex-check
etex-check: etrip etex.efmt
# Test truncation (but don't bother showing the warning msg).
	.\$(objdir)\etex --progname=etex --output-comment="$(outcom)" \
	  $(srcdir)/tests/hello \
	  && .\$(objdir)\dvitype hello.dvi | grep olaf@infovore.xs4all.nl >nul
# \openout should show up in \write's.
	.\$(objdir)\etex --progname=etex $(srcdir)/tests/openout && grep xfoo openout.log
# one.two.tex -> one.two.log
	.\$(objdir)\etex --progname=etex $(srcdir)/tests/one.two && dir /w one.two.log
# uno.dos -> uno.log
	.\$(objdir)\etex --progname=etex $(srcdir)/tests/uno.dos && dir /w uno.log
	.\$(objdir)\etex --progname=etex $(srcdir)/tests/just.texi && dir /w just.log
	-.\$(objdir)\etex --progname=etex $(srcdir)/tests/batch.tex
	.\$(objdir)\etex --progname=etex --shell $(srcdir)/tests/write18 | grep echo
# tcx files are a bad idea.
#	.\$(objdir)\etex --translate-file=$(srcdir)/share/isol1-t1.tcx \
#	  $(srcdir)/tests/eight && ./dvitype eight.dvi >eigh.typ
	.\$(objdir)\etex --mltex --progname=einitex $(srcdir)/tests/mltextst
	-.\$(objdir)\etex --progname=etex <nul
#	-PATH=$(kpathsea_dir):$$PATH .\$(objdir)\etex '\nonstopmode\font\foo=nonesuch\end'
	set PATH=$(kpathsea_dir);$(kpathsea_srcdir);$(PATH)
	set WEB2C=$(kpathsea_srcdir)
	set TMPDIR=..
	-.\$(objdir)\etex --progname=etex "\nonstopmode\font\foo=nonesuch\end"


# Cleaning up.
clean:: etex-clean
etex-clean: etrip-clean
#	$(LIBTOOL) --mode=clean $(RM) etex
	-@echo $(verbose) & ( \
		for %%i in ($(etex_o) $(etex_c) etexextra.c etexcoerce.h etexd.h \
			    etex.p etex.pool etex.web etex.ch \
			    etex.efmt etex.log \
			    hello.dvi hello.log xfoo.out openout.log one.two.log uno.log \
			    just.log batch.log write18.log mltextst.log texput.log \
			    missfont.log) do $(del) %%i $(redir_stderr) \
	)
	-@$(deldir) tfm $(redir_stderr)

#etrip
etestdir = $(srcdir)\etexdir\etrip

etrip: $(objdir)\pltotf.exe $(objdir)\tftopl.exe $(objdir)\etex.exe $(objdir)\dvitype.exe etrip-clean
	@echo ">>> See $(etestdir)\etrip.diffs for example of acceptable diffs."
	@echo "*** TRIP test for e-TeX in compatibility mode ***."
	.\$(objdir)\pltotf $(testdir)\trip.pl trip.tfm
	.\$(objdir)\tftopl .\trip.tfm trip.pl
	-$(diff) $(testdir)\trip.pl trip.pl
	-$(del) \f trip.tex & $(copy) $(testdir)\trip.tex . # get same filename in log
# FIXME: this works only written on 2 lines !!!
	set TEXMFCNFOLD=$(TEXMFCNF)
	set TEXMFCNF=$(etestdir)
	-.\$(objdir)\etex --progname=einitex <$(testdir)\trip1.in >ctripin.fot
	$(copy) trip.log ctripin.log & $(del) trip.log
	-$(diff) $(testdir)\tripin.log ctripin.log
	-.\$(objdir)\etex <$(testdir)\trip2.in >ctrip.fot
	$(copy) trip.log ctrip.log & $(del) trip.log
	-$(diff) $(testdir)\trip.fot ctrip.fot
	-$(diff) $(diffflags) $(testdir)\trip.log ctrip.log
	.\$(objdir)\dvitype $(dvitype_args) trip.dvi >ctrip.typ
	-$(diff) $(diffflags) $(testdir)\trip.typ ctrip.typ
	@echo "*** TRIP test for e-TeX in extended mode ***."
	-.\$(objdir)\etex -progname=einitex <$(etestdir)\etrip1.in >xtripin.fot
	$(copy) trip.log xtripin.log & $(del) trip.log
	-$(diff) ctripin.log xtripin.log
	-.\$(objdir)\etex <$(etestdir)\trip2.in >xtrip.fot
	$(copy) trip.log xtrip.log & $(del) trip.log
	-$(diff) ctrip.fot xtrip.fot
	-$(diff) $(diffflags) ctrip.log xtrip.log
	.\$(objdir)\dvitype $(dvitype_args) trip.dvi >xtrip.typ
	-$(diff) $(diffflags) ctrip.typ xtrip.typ
	@echo "*** e-TeX specific part of e-TRIP test ***."
	.\$(objdir)\pltotf $(etestdir)\etrip.pl etrip.tfm
	.\$(objdir)\tftopl .\etrip.tfm etrip.pl
	-$(diff) $(etestdir)\etrip.pl etrip.pl
# get same filename in log
	-$(del) \f etrip.tex & $(copy) $(etestdir:/=\)\etrip.tex . 
	-.\$(objdir)\etex --progname=einitex <$(etestdir)\etrip2.in >etripin.fot
	$(copy) etrip.log etripin.log & $(del) etrip.log
	-$(diff) $(etestdir)\etripin.log etripin.log
	-.\$(objdir)\etex <$(etestdir)\etrip3.in >etrip.fot
	-$(diff) $(etestdir)\etrip.fot etrip.fot
	-$(diff) $(diffflags) $(etestdir)\etrip.log etrip.log
	set TEXMFCNF=$(TEXMFCNFOLD)

# Cleaning up for the etrip.
etrip-clean:
	-@echo $(verbose) & ( \
		for %%i in (trip.tfm trip.pl trip.tex trip.efmt ctripin.fot ctripin.log \
			    ctrip.fot ctrip.log trip.dvi ctrip.typ \
			    xtripin.fot xtripin.log \
			    xtrip.fot xtrip.log xtrip.typ \
			    etrip.tfm etrip.pl etrip.tex etrip.efmt etripin.fot etripin.log \
			    etrip.fot etrip.log etrip.dvi etrip.out etrip.typ \
			    tripos.tex 8terminal.tex) do $(del) %%i $(redir_stderr) \
	)
	-@$(deldir) tfm $(redir_stderr)

# Distfiles ...
# triptrapdiffs: etexdir\etrip\etrip.diffs
# etexdir\etrip\etrip.diffs: etex
#	$(make) etrip | tail +1 >etexdir\etrip\etrip.diffs

# Dumps
# all_efmts = etex.efmt $(efmts)
# 
# dumps: efmts
# efmts: $(all_efmts)
# 
# etex.efmt: $(etex)
# 	$(dumpenv) $(make) progname=etex files="etex.src plain.tex cmr10.tfm" prereq-check
# 	$(dumpenv) .\$(objdir)\etex --progname=etex --jobname=etex --ini "*\input etex.src \dump" < nul
# 
# elatex.efmt: $(etex)
# 	$(dumpenv) $(make) progname=elatex files="latex.ltx" prereq-check
# 	$(dumpenv) .\$(objdir)\etex --progname=elatex --jobname=elatex --ini "*\input latex.ltx" < nul
# 
# latex.efmt: $(etex)
# 	$(dumpenv) $(make) progname=latex files="latex.ltx" prereq-check
# 	$(dumpenv) .\$(objdir)\etex --progname=latex --jobname=latex --ini "*\input latex.ltx" < nul
# 
# tex.efmt: $(etex)
# 	$(dumpenv) $(make) progname=tex files="plain.tex cmr10.tfm" prereq-check
# 	$(dumpenv) .\$(objdir)\etex --progname=tex --jobname=tex --ini "\input plain \dump" < nul
# 
# olatex.efmt: $(etex)
# 	$(dumpenv) $(make) progname=olatex files="latex.ltx" prereq-check
# 	$(dumpenv) .\$(objdir)\etex --progname=olatex --progname=olatex --ini "\input latex.ltx" < nul

# Install
install-etex: install-etex-exec install-etex-data
install-etex-exec: install-etex-links
# install-etex-data: install-etex-dumps
install-etex-dumps: install-etex-fmts

install-programs: install-etex-programs
install-etex-programs: $(etex) $(bindir)
	-@echo $(verbose) & ( \
	  for %%p in ($(etex)) do $(copy) %%p $(bindir) \
	) $(redir_stdout)

# install-links: install-etex-links
install-etex-links: install-etex-programs
#	-@echo $(verbose) & ( \
#	  pushd $(bindir) & \
#	    $(del) .\einitex.exe .\evirtex.exe & \
#	    $(lnexe) .\etex.exe $(bindir)\einitex.exe & \
#	    $(lnexe) .\etex.exe $(bindir)\evirtex.exe & \
#	  popd \
#	) $(redir_stdout)
	-@echo $(verbose) & ( \
	  if NOT "$(efmts)"=="" \
	    for %%i in ($(efmts)) do \
              pushd $(bindir) & \
                $(del) .\%%~ni.exe & \
	        $(lnexe) .\etex.exe $(bindir)\%%~ni.exe & \
	      popd \
	) $(redir_stdout)

# install-fmts: install-etex-fmts
install-etex-fmts: efmts $(fmtdir)
	-@echo $(verbose) & ( \
	  for %%f in ($(all_efmts)) \
	    do $(copy) %%f $(fmtdir)\%%f \
	) $(redir_stdout)

install-data:: install-etex-data
install-etex-data: $(texpooldir)
	@$(copy) etex.pool $(texpooldir)\etex.pool $(redir_stdout)

# End of etex.mk.
# 
# Local variables:
# page-delimiter: ""
# mode: Makefile
# End:
