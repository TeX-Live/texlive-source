################################################################################
#
# Makefile  : Web2C
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <04/03/20 16:25:18 popineau>
#
################################################################################

# Makefile for web2c --kb@mail.tug.org. Public domain.
version = 7.5.1

# These lines define the memory dumps that fmts/bases/mems will make and
# install-fmts/install-bases/install-mems will install. plain.* is
# created automatically (as a link).  See the Formats node in
# doc/web2c.texi for details on the fmts.
#FMU fmts = olatex.fmt # amstex.fmt eplain.fmt texinfo.fmt
#FMU efmts = latex.efmt
#FMU pdffmts = pdfolatex.fmt
#FMU pdfefmts = pdflatex.efmt
#FMU pdfxfmts = pdflatex.xfmt
#FMU ofmts = lambda.oft
#FMU eofmts = elambda.eoft
#FMU bases = # I do not recommend building cmmf.base.
#FMU mems =  # mfplain.mem is probably not generally useful.

# The name of the file that defines your local devices for Metafont.
# (Only relevant during `make bases'.)  I recommend `modes.mf', which
# defines all known modes, plus useful definitions for all fonts.  It's
# available from ftp://ftp.tug.org/tex/modes.mf, among other places.
localmodes = modes

# Used for triptrap.
DIFF = diff
DIFFFLAGS =

root_srcdir=..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

USE_KPATHSEA = 1
USE_GETURL = 1
USE_GNUW32 = 1
USE_ZLIB = 1
USE_PNG=1
USE_JPEG = 1
USE_XPDF = 1
USE_TEX = 1
MAKE_TEX = 1

!include <msvc/common.mak>

# omegaversion = -1.15
!ifdef omegaversion
DEFS = $(DEFS) -DOLD_OMEGA
!endif

# Compilation options.
DEFS = -I. $(DEFS) -DHAVE_CONFIG_H -DOEM -DJOBTIME -DTIME_STATS \
	-DOUTPUT_DIR -DHALT_ON_ERROR

!ifdef TEX_DLL
DEFS = $(DEFS) -DMAKE_TEX_DLL
!endif

# With --enable-ipc, TeX may need to link with -lsocket.
socketlibs = delayimp.lib /delayload:wsock32.dll # @socketlibs@

proglib = lib\$(objdir)\lib.lib
windowlib = window\$(objdir)\window.lib
pdflib = pdftexdir\$(objdir)\libpdf.lib
pdftexlibs = $(pdflib) $(pnglib) $(zliblib) $(xpdflib)
pdftexlibsdep = $(pdflib) $(png) $(zlib) $(xpdf)

# The .bat script that does the conversion:
web2c = web2c\convert.bat $(objdir)
# Additional dependencies:
web2c_common = web2c\convert.bat web2c\common.defines
web2c_programs = web2c\$(objdir)\fixwrites.exe	\
#	$(msvcdir)\splitup.pl			\
#	web2c\$(objdir)\splitup.exe		\
	web2c\$(objdir)\web2c.exe
web2c_texmf = $(web2c_common) $(web2c_programs) web2c\texmf.defines web2c\coerce.h

# Calling tangle
# tangle = WEBINPUTS=$(srcdir) ./tangle
tangle = $(objdir)\tangle.exe

# The environment for making dumps.
# dumpenv = TEXMFCNF=../kpathsea TEXMF=$(texmf)

# Unfortunately, suffix rules can't have dependencies, or multiple
# targets, and we can't assume all makes supports pattern rules.
.SUFFIXES: .p .c .ch .p .res .rc
.p.c: # really depends on $(web2c_programs), and generates .h.
	 $(web2c) $*
.ch.p: # really depends on ./tangle; for mf/mp/tex, also generates .pool
	.\$(objdir)\tangle $*.web $<

# Prevent Make from deleting the intermediate forms.
.PRECIOUS: %.ch %.p %.c

mf = $(objdir)\mf.exe
mfn = $(objdir)\mf-nowin.exe
mpost = $(objdir)\mpost.exe
tex = $(objdir)\tex.exe
!ifdef TEX_DLL
mf = $(mf) $(objdir)\$(library_prefix)mf.dll
mfn = $(mfn) $(objdir)\$(library_prefix)mf-nowin.dll
mpost = $(mpost) $(objdir)\$(library_prefix)mpost.dll
tex = $(tex) $(objdir)\$(library_prefix)tex.dll
!endif

mpware = mpware\$(objdir)\dmp.exe ..\contrib\$(objdir)\makempx.exe mpware\$(objdir)\mpto.exe mpware\$(objdir)\newer.exe
mpware_sources = mpware\dmp.c ..\contrib\makempx.c mpware\mpto.c mpware\newer.c

#  
default all: programs manpages doc\web2c.info #FMU dumps
check: dumps

$(objdir)\bibtex.exe: $(objdir)\bibtex.obj $(objdir)\bibtex.res $(kpathsealib) $(proglib)
	$(link) $(**) $(conlibs)
bibtex.c bibtex.h: $(web2c_common) $(web2c_programs) web2c\cvtbib.sed bibtex.p
	$(web2c) bibtex
bibtex.p: $(tangle) bibtex.web bibtex.ch
	$(tangle) bibtex bibtex
check: bibtex-check
bibtex-check: $(objdir)\bibtex.exe
#	if not exist tests\exampl.aux $(copy) $(srcdir)\tests\exampl.aux tests\exampl.aux
	@set BSTINPUTS=$(srcdir)\tests
	@set TEXMFCNF=..\kpathsea\texmf.cnf
	.\$(objdir)\bibtex tests\exampl
	@set TEXMFCNF=
	@set BSTINPUTS=
clean:: bibtex-clean
bibtex-clean:
#	$(LIBTOOL) --mode=clean $(del) bibtex
	-@echo $(verbose) & ( \
		for %%i in ($(objdir)\bibtex.obj bibtex.c bibtex.h bibtex.p) do $(del) %%i $(redir_stderr) \
	)
	-@$(del) tests\exampl.blg tests\exampl.bbl $(redir_stderr)

$(objdir)\cweave.exe: $(objdir)\cweave.obj $(objdir)\cweb.obj
	$(link) $(**) $(conlibs)
cweave.c: $(objdir)\ctangle.exe cwebdir\cweave.w cwebdir\cweav-w2c.ch
	@set CWEBINPUTS=$(srcdir)\cwebdir 
	.\$(objdir)\ctangle cweave cweav-w2c
	@set CWEBINPUTS=
check: cweave-check
cweave-check: $(objdir)\cweave.exe
	@set CWEBINPUTS=$(srcdir)\cwebdir 
	.\$(objdir)\cweave common.w
	@set CWEBINPUTS=
clean:: cweave-clean
cweave-clean:
#	$(LIBTOOL) --mode=clean $(del) cweave
	-@echo $(verbose) & ( \
		for %%i in ($(objdir)\cweave.obj cweave.c) do $(del) %%i $(redir_stderr) \
	)
	-@$(del) common.tex common.scn common.idx $(redir_stderr)

$(objdir)\dvicopy.exe: $(objdir)\dvicopy.obj $(objdir)\dvicopy.res $(kpathsealib) $(proglib)
	$(link) $(**) $(conlibs)
dvicopy.c dvicopy.h: $(web2c_common) $(web2c_programs) dvicopy.p
	$(web2c) dvicopy
dvicopy.p: $(tangle) dvicopy.web dvicopy.ch
	$(tangle) dvicopy dvicopy
check: dvicopy-check
dvicopy-check: $(objdir)\dvicopy.exe
	.\$(objdir)\dvicopy $(srcdir)\tests\story tests\xstory.dvi
# Redirect stderr so the terminal output will end up in the log file.
	@set TFMFONTS=$(srcdir)\tests
	@set VFFONTS=$(srcdir)\tests
	.\$(objdir)\dvicopy < $(srcdir)\tests\pplr.dvi > tests\xpplr.dvi
	@set VFFONTS=
	@set TFMFONTS=
clean:: dvicopy-clean
dvicopy-clean:
#	$(LIBTOOL) --mode=clean $(del) dvicopy
	-@echo $(verbose) & ( \
		for %%i in ($(objdir)\dvicopy.obj dvicopy.c dvicopy.h dvicopy.p) do $(del) %%i $(redir_stderr) \
	)
	-@$(del) tests\xstory.dvi tests\xpplr.dvi $(redir_stderr)

$(objdir)\dvitomp.exe: $(objdir)\dvitomp.obj $(objdir)\dvitomp.res $(kpathsealib) $(proglib)
	$(link) $(**) $(conlibs)
dvitomp.c dvitomp.h: $(web2c_common) $(web2c_programs) dvitomp.p
	$(web2c) dvitomp
dvitomp.p: $(tangle) dvitomp.web dvitomp.ch
	$(tangle) dvitomp dvitomp
check: dvitomp-check
dvitomp-check: $(objdir)\dvitomp.exe
	@set TFMFONTS=$(srcdir)\tests
	@set VFFONTS=$(srcdir)\tests
	.\$(objdir)\dvitomp $(srcdir)\tests\story.dvi tests\xstory.mpx
	.\$(objdir)\dvitomp $(srcdir)\tests\ptmr
	$(move) ptmr.mpx tests\xptmr.mpx
	@set TFMFONTS=
	@set VFFONTS=
clean:: dvitomp-clean
dvitomp-clean:
#	$(LIBTOOL) --mode=clean $(del) dvitomp
	-@echo $(verbose) & ( \
		for %%i in ($(objdir)\dvitomp.obj dvitomp.c dvitomp.h dvitomp.p) do $(del) %%i $(redir_stderr) \
	)
	-@$(del) tests\xstory.mpx tests\xptmr.mpx $(redir_stderr)

$(objdir)\dvitype.exe: $(objdir)\dvitype.obj $(objdir)\dvitype.res $(kpathsealib) $(proglib)
	$(link) $(**) $(conlibs)
dvitype.c dvitype.h: $(web2c_common) $(web2c_programs) dvitype.p
	$(web2c) dvitype
dvitype.p: $(tangle) dvitype.web dvitype.ch
	$(tangle) dvitype dvitype
check: dvitype-check
dvitype-check: $(objdir)\dvitype.exe
	.\$(objdir)\dvitype -show-opcodes $(srcdir)\tests\story >tests\xstory.dvityp
	.\$(objdir)\dvitype -p=*.*.2 $(srcdir)\tests\pagenum.dvi >tests\xpagenum.typ
clean:: dvitype-clean
dvitype-clean:
#	$(LIBTOOL) --mode=clean $(del) dvitype
	-@echo $(verbose) & ( \
		for %%i in ($(objdir)\dvitype.obj dvitype.c dvitype.h dvitype.p) do $(del) %%i $(redir_stderr) \
	)
	-@$(del) tests\xstory.dvityp tests\xpagenum.typ $(redir_stderr)

$(objdir)\gftodvi.exe: $(objdir)\gftodvi.obj $(objdir)\gftodvi.res $(kpathsealib) $(proglib)
	$(link) $(**) $(conlibs)
gftodvi.c gftodvi.h: $(web2c_common) $(web2c_programs) gftodvi.p
	$(web2c) gftodvi
gftodvi.p: $(tangle) gftodvi.web gftodvi.ch
	$(tangle) gftodvi gftodvi
check: gftodvi-check
gftodvi-check: $(objdir)\gftodvi.exe
	set TFMFONTS=$(srcdir)\tests
	.\$(objdir)\gftodvi -verbose $(srcdir)\tests\cmr10.600gf
	$(move) cmr10.dvi tests\xcmr10.dvi
clean:: gftodvi-clean
gftodvi-clean:
#	$(LIBTOOL) --mode=clean $(del) gftodvi
	-@echo $(verbose) & ( \
		for %%i in ($(objdir)\gftodvi.obj gftodvi.c gftodvi.h gftodvi.p) do $(del) %%i $(redir_stderr) \
	)
	-@$(del) tests\xcmr10.dvi $(redir_stderr)

$(objdir)\gftopk.exe: $(objdir)\gftopk.obj $(objdir)\gftopk.res $(kpathsealib) $(proglib)
	$(link) $(**) $(conlibs)
gftopk.c gftopk.h: $(web2c_common) $(web2c_programs) gftopk.p
	$(web2c) gftopk
gftopk.p: $(tangle) gftopk.web gftopk.ch
	$(tangle) gftopk gftopk
check: gftopk-check
gftopk-check: $(objdir)\gftopk.exe
	.\$(objdir)\gftopk -verbose $(srcdir)\tests\cmr10.600gf tests\xcmr10.pk
	.\$(objdir)\gftopk $(srcdir)\tests\cmr10.600gf cmr10.pk & $(del) cmr10.pk
clean:: gftopk-clean
gftopk-clean:
#	$(LIBTOOL) --mode=clean $(del) gftopk
	-@echo $(verbose) & ( \
		for %%i in ($(objdir)\gftopk.obj gftopk.c gftopk.h gftopk.p) do $(del) %%i $(redir_stderr) \
	)
	-@$(del) tests\xcmr10.pk $(redir_stderr)

$(objdir)\gftype.exe: $(objdir)\gftype.obj $(objdir)\gftype.res $(kpathsealib) $(proglib)
	$(link) $(**) $(conlibs)
gftype.c gftype.h: $(web2c_common) $(web2c_programs) gftype.p
	$(web2c) gftype
gftype.p: $(tangle) gftype.web gftype.ch
	$(tangle) gftype gftype
check: gftype-check
gftype-check: $(objdir)\gftype.exe
	.\$(objdir)\gftype $(srcdir)\tests\cmr10.600gf >tests\xcmr10.gft1
	.\$(objdir)\gftype -m -i $(srcdir)\tests\cmr10.600gf >tests\xcmr10.gft2
clean:: gftype-clean
gftype-clean:
#	$(LIBTOOL) --mode=clean $(del) gftype
	-@echo $(verbose) & ( \
		for %%i in ($(objdir)\gftype.obj gftype.c gftype.h gftype.p) do $(del) %%i $(redir_stderr) \
	)
	-@$(del) tests\xcmr10.gft1 tests\xcmr10.gft2 $(redir_stderr)

$(objdir)\mft.exe: $(objdir)\mft.obj $(objdir)\mft.res $(kpathsealib) $(proglib)
	$(link) $(**) $(conlibs)
mft.c mft.h: $(web2c_common) $(web2c_programs) mft.p
	$(web2c) mft
mft.p: $(tangle) mft.web mft.ch
	$(tangle) mft mft
check: mft-check
mft-check: $(objdir)\mft.exe
	.\$(objdir)\mft $(srcdir)\tests\io & $(move) io.tex tests\io.tex
clean:: mft-clean
mft-clean:
#	$(LIBTOOL) --mode=clean $(del) mft
	-@echo $(verbose) & ( \
		for %%i in ($(objdir)\mft.obj mft.c mft.h mft.p) do $(del) %%i $(redir_stderr) \
	)
	-@$(del) tests\io.tex $(redir_stderr)

$(objdir)\patgen.exe: $(objdir)\patgen.obj $(objdir)\patgen.res $(kpathsealib) $(proglib)
	$(link) $(**) $(conlibs)
patgen.c patgen.h: $(web2c_common) $(web2c_programs) patgen.p
	$(web2c) patgen
patgen.p: $(tangle) patgen.web patgen.ch
	$(tangle) patgen patgen
check: patgen-check
patgen-check: $(objdir)\patgen.exe
	.\$(objdir)\patgen $(srcdir)\tests\dict $(srcdir)\tests\patterns tests\xout \
	   $(srcdir)\tests\translate <$(srcdir)\tests\patgen.in
clean:: patgen-clean
patgen-clean:
#	$(LIBTOOL) --mode=clean $(del) patgen
	-@echo $(verbose) & ( \
		for %%i in ($(objdir)\patgen.obj patgen.c patgen.h patgen.p) do $(del) %%i $(redir_stderr) \
	)
	-@$(del) tests\xout pattmp.1 $(redir_stderr)

$(objdir)\pktogf.exe: $(objdir)\pktogf.obj $(objdir)\pktogf.res $(kpathsealib) $(proglib)
	$(link) $(**) $(conlibs)
pktogf.c pktogf.h: $(web2c_common) $(web2c_programs) pktogf.p
	$(web2c) pktogf
pktogf.p: $(tangle) pktogf.web pktogf.ch
	$(tangle) pktogf pktogf
check: pktogf-check
pktogf-check: $(objdir)\pktogf.exe
	.\$(objdir)\pktogf -verbose $(srcdir)\tests\cmr10.pk tests\xcmr10.600gf
	.\$(objdir)\pktogf $(srcdir)\tests\cmr10.pk & rm cmr10.gf
clean:: pktogf-clean
pktogf-clean:
#	$(LIBTOOL) --mode=clean $(del) pktogf
	-@echo $(verbose) & ( \
		for %%i in ($(objdir)\pktogf.obj pktogf.c pktogf.h pktogf.p) do $(del) %%i $(redir_stderr) \
	)
	-@$(del) tests\xcmr10.600gf $(redir_stderr)

$(objdir)\pktype.exe: $(objdir)\pktype.obj $(objdir)\pktype.res $(kpathsealib) $(proglib)
	$(link) $(**) $(conlibs)
pktype.c pktype.h: $(web2c_common) $(web2c_programs) pktype.p
	$(web2c) pktype
pktype.p: $(tangle) pktype.web pktype.ch
	$(tangle) pktype pktype
check: pktype-check
pktype-check: $(objdir)\pktype.exe
	.\$(objdir)\pktype $(srcdir)\tests\cmr10.pk >tests\xcmr10.pktyp
clean:: pktype-clean
pktype-clean:
#	$(LIBTOOL) --mode=clean $(del) pktype
	-@echo $(verbose) & ( \
		for %%i in ($(objdir)\pktype.obj pktype.c pktype.h pktype.p) do $(del) %%i $(redir_stderr) \
	)
	-@$(del) tests\xcmr10.pktyp $(redir_stderr)

$(objdir)\pltotf.exe: $(objdir)\pltotf.obj $(objdir)\pltotf.res $(kpathsealib) $(proglib)
	$(link) $(**) $(conlibs)
pltotf.c pltotf.h: $(web2c_common) $(web2c_programs) pltotf.p
	$(web2c) pltotf
pltotf.p: $(tangle) pltotf.web pltotf.ch
	$(tangle) pltotf pltotf
check: pltotf-check
pltotf-check: $(objdir)\pltotf.exe
	.\$(objdir)\pltotf -verbose $(srcdir)\tests\cmr10 tests\xcmr10
clean:: pltotf-clean
pltotf-clean:
#	$(LIBTOOL) --mode=clean $(del) pltotf
	-@echo $(verbose) & ( \
		for %%i in ($(objdir)\pltotf.obj pltotf.c pltotf.h pltotf.p) do $(del) %%i $(redir_stderr) \
	)
	-@$(del) tests\xcmr10.tfm $(redir_stderr)

$(objdir)\pooltype.exe: $(objdir)\pooltype.obj $(objdir)\pooltype.res $(kpathsealib) $(proglib)
	$(link) $(**) $(conlibs)
pooltype.c pooltype.h: $(web2c_common) $(web2c_programs) pooltype.p
	$(web2c) pooltype
pooltype.p: $(tangle) pooltype.web pooltype.ch
	$(tangle) pooltype pooltype
check: pooltype-check
pooltype-check: tex.pool
pooltype-check: $(objdir)\pooltype.exe
	.\$(objdir)\pooltype tex.pool >tests\xtexpool.typ
clean:: pooltype-clean
pooltype-clean:
#	$(LIBTOOL) --mode=clean $(del) pooltype
	-@echo $(verbose) & ( \
		for %%i in ($(objdir)\pooltype.obj pooltype.c pooltype.h pooltype.p) do $(del) %%i $(redir_stderr) \
	)
	-@$(del) tests\xtexpool.typ $(redir_stderr)

$(objdir)\tie.exe: $(objdir)\tie.obj $(kpathsealib) $(proglib)
	$(link) $(**) $(conlibs)
tie.c: $(objdir)\ctangle.exe tiedir\tie.w tiedir\tie-w2c.ch
	@set CWEBINPUTS=$(srcdir)\tiedir 
	.\$(objdir)\ctangle tie.w tie-w2c.ch
	@set CWEBINPUTS=
clean:: tie-clean
tie-clean:
#	$(LIBTOOL) --mode=clean $(del) tie
	-@echo $(verbose) & ( \
		for %%i in ($(objdir)\tie.obj tie.c) do $(del) %%i $(redir_stderr) \
	)

$(objdir)\tftopl.exe: $(objdir)\tftopl.obj $(objdir)\tftopl.res $(kpathsealib) $(proglib)
	$(link) $(**) $(conlibs)
tftopl.c tftopl.h: $(web2c_common) $(web2c_programs) tftopl.p
	$(web2c) tftopl
tftopl.p: $(tangle) tftopl.web tftopl.ch
	$(tangle) tftopl tftopl
check: tftopl-check
tftopl-check: $(objdir)\tftopl.exe
	.\$(objdir)\tftopl -verbose $(srcdir)\tests\cmr10 tests\xcmr10
clean:: tftopl-clean
tftopl-clean:
#	$(LIBTOOL) --mode=clean $(del) tftopl
	-@echo $(verbose) & ( \
		for %%i in ($(objdir)\tftopl.obj tftopl.c tftopl.h tftopl.p) do $(del) %%i $(redir_stderr) \
	)
	-@$(del) tests\xcmr10.pl $(redir_stderr)

$(objdir)\vftovp.exe: $(objdir)\vftovp.obj $(objdir)\vftovp.res $(kpathsealib) $(proglib)
	$(link) $(**) $(conlibs)
vftovp.c vftovp.h: $(web2c_common) $(web2c_programs) vftovp.p
	$(web2c) vftovp
vftovp.p: $(tangle) vftovp.web vftovp.ch
	$(tangle) vftovp vftovp
check: vftovp-check
vftovp-check: $(objdir)\vftovp.exe
	@set TFMFONTS=$(srcdir)\tests
	.\$(objdir)\vftovp -verbose $(srcdir)\tests\ptmr ptmr tests\xptmr
	@set TFMFONTS=
clean:: vftovp-clean
vftovp-clean:
#	$(LIBTOOL) --mode=clean $(del) vftovp
	-@echo $(verbose) & ( \
		for %%i in ($(objdir)\vftop.obj vftovp.c vftovp.h vftovp.p) do $(del) %%i $(redir_stderr) \
	)
	-@$(del) tests\xptmr.vpl $(redir_stderr)

$(objdir)\vptovf.exe: $(objdir)\vptovf.obj $(objdir)\vptovf.res $(kpathsealib) $(proglib)
	$(link) $(**) $(conlibs)
vptovf.c vptovf.h: $(web2c_common) $(web2c_programs) vptovf.p
	$(web2c) vptovf
vptovf.p: $(tangle) vptovf.web vptovf.ch
	$(tangle) vptovf vptovf
check: vptovf-check
vptovf-check: $(objdir)\vptovf.exe
	.\$(objdir)\vptovf $(srcdir)\tests\ptmr tests\xptmr tests\xptmr
clean:: vptovf-clean
vptovf-clean:
#	$(LIBTOOL) --mode=clean $(del) vptovf
	-@echo $(verbose) & ( \
		for %%i in ($(objdir)\vptovf.obj vptovf.c vptovf.h vptovf.p) do $(del) %%i $(redir_stderr) \
	)
	-@$(del) tests\xptmr.vf tests\xptmr.tfm $(redir_stderr)

$(objdir)\weave.exe: $(objdir)\weave.obj $(objdir)\weave.res $(kpathsealib) $(proglib)
	$(link) $(**) $(conlibs)
weave.c weave.h: $(web2c_common) $(web2c_programs) weave.p
	$(web2c) weave
weave.p: $(tangle) weave.web weave.ch
	$(tangle) weave weave
check: weave-check
weave-check: $(objdir)\weave.exe
	.\$(objdir)\weave $(srcdir)\pooltype
clean:: weave-clean
weave-clean:
#	$(LIBTOOL) --mode=clean $(del) weave
	-@echo $(verbose) & ( \
		for %%i in ($(objdir)\weave.obj weave.c weave.h weave.p) do $(del) %%i $(redir_stderr) \
	)
	-@$(del) pooltype.tex $(redir_stderr)

# 
# Defines that are common to the texmf programs.

trip = trip
etrip = etrip
triptrap: $(trip) trap mptrap $(etrip)
testdir = $(srcdir)\triptrap
testenv = TEXMFCNF=$(testdir)
dvitype_args = -output-level=2 -dpi=72.27 -page-start="*.*.*.*.*.*.*.*.*.*"

# 
# Metafont
mf_c = mf.c
mf_o = $(objdir)\mf.obj
mfn_o = $(objdir)\mfnowin.obj
!ifdef TEX_DLL
$(objdir)\$(library_prefix)mf.exp: $(objdir)\$(library_prefix)mf.lib

$(objdir)\$(library_prefix)mf.lib: $(mf_o)
	$(archive) /DEF $(mf_o)

$(objdir)\$(library_prefix)mf.dll: $(mf_o) $(objdir)\$(library_prefix)mf.exp $(objdir)\mf.res $(windowlib) $(kpathsealib) $(proglib)
	$(link_dll) $(**) gdi32.lib $(conlibs)

$(objdir)\mf.exe: $(objdir)\win32main.obj $(objdir)\$(library_prefix)mf.lib $(proglib)
	$(link) $(**) $(conlibs)
!else
$(objdir)\mf.exe: $(mf_o) $(objdir)\win32main.obj $(windowlib) $(objdir)\mf.res $(kpathsealib) $(proglib)
	$(link) $(**) gdi32.lib $(conlibs)
!endif

!ifdef TEX_DLL
$(objdir)\$(library_prefix)mf-nowin.exp: $(objdir)\$(library_prefix)mf-nowin.lib

$(objdir)\$(library_prefix)mf-nowin.lib: $(mf_o)	
	$(archive) /DEF $(mf_o)

$(objdir)\$(library_prefix)mf-nowin.dll: $(mfn_o) window\$(objdir)\trap.obj $(objdir)\$(library_prefix)mf-nowin.exp $(objdir)\mf.res $(kpathsealib) $(proglib)
	$(link_dll) $(**) gdi32.lib $(conlibs)

$(objdir)\mf-nowin.exe: $(objdir)\win32main.obj $(objdir)\$(library_prefix)mf-nowin.lib $(proglib)
	$(link) $(**) $(conlibs)
!else
$(objdir)\mf-nowin.exe: $(mfn_o) window\$(objdir)\trap.obj $(objdir)\win32main.obj $(objdir)\mf.res $(kpathsealib) $(proglib)
	$(link) $(**) gdi32.lib $(conlibs)
!endif

$(objdir)\mfnowin.obj: mf.c
	$(compile) -DMFNOWIN mf.c
$(mf_c) mfcoerce.h mfd.h: mf.p $(web2c_texmf) web2c\cvtmf1.sed web2c\cvtmf2.sed
	$(web2c) mf
mf.p mf.pool: $(tangle) mf.web mf-w32.ch
	$(tangle) mf.web mf-w32.ch
mf-w32.ch: $(objdir)\tie.exe mf.ch mf-supp-w32.ch
	$(objdir)\tie.exe -c mf-w32.ch mf.web mf.ch mf-supp-w32.ch
check: mf-check
mf-check: trap mf.base
	.\$(objdir)\mf --progname=mf "&./mf \tracingstats:=1; end."
	.\$(objdir)\mf --progname=mf "&./mf $(srcdir)\tests\online"
	.\$(objdir)\mf --progname=mf "&./mf $(srcdir)\tests\one.two"
	.\$(objdir)\mf --progname=mf "&./mf $(srcdir)\tests\uno.dos"
clean:: mf-clean
mf-clean: trap-clean
	-@echo $(verbose) & ( \
		for %%i in ($(mf_c) mfcoerce.h mfd.h mf.p mf.pool  \
                            $(mfw_o) mfextra.c $(mfn_o) mfnowin.c  \
                            mf.base mf.log mfput.log online.2602gf \
                            online.log one.two.log uno.log) do $(del) %%i $(redir_stderr) \
	)
#	$(LIBTOOL) --mode=clean $(del) mf
#	$(LIBTOOL) --mode=clean $(del) mfw
#	$(LIBTOOL) --mode=clean $(del) mf-nowin

# Can't run trap and mptrap in parallel, because both write trap.{log,tfm}.
trap: mf tftopl gftype trap-clean
trap: $(objdir)\mf.exe $(objdir)\tftopl.exe $(objdir)\gftype.exe
	@echo ">>> See $(testdir)\mftrap.diffs for example of acceptable diffs."
	set TEXMFCNFOLD=$(TEXMFCNF)
	set TEXMFCNF=$(testdir)
# get same filename in log
	-@$(del) trap.mf & $(copy) $(testdir)\trap.mf . $(redir_stderr)
	-.\$(objdir)\mf -progname=inimf < $(testdir)\mftrap1.in > mftrapin.fot
	$(move) trap.log mftrapin.log
	-$(diff) $(testdir)\mftrapin.log mftrapin.log
	-.\$(objdir)\mf -progname=inimf <$(testdir)\mftrap2.in >mftrap.fot
	$(move) trap.log mftrap.log
	$(move) trap.tfm mftrap.tfm
	-$(diff) $(testdir)\mftrap.fot mftrap.fot
	-$(diff) $(testdir)\mftrap.log mftrap.log
	.\$(objdir)\tftopl .\mftrap.tfm mftrap.pl
	-$(diff) $(testdir)\mftrap.pl mftrap.pl
	-.\$(objdir)\gftype -m -i .\trap.72270gf >trap.typ
	-$(diff) $(testdir)\trap.typ trap.typ
	set TEXMFCNF=$(TEXMFCNFOLD)

trap-clean:
	-@echo $(verbose) & ( \
		for %%i in (trap.mf trap.base mftrapin.fot mftrapin.log \
                            mftrap.fot mftrap.log mftrap.tfm mftrap.pl  \
			    trap.72270gf trap.typ) do $(del) %%i $(redir_stderr) \
	)
# 
# MetaPost
mp_c = mp.c
mp_o = $(objdir)\mp.obj
!ifdef TEX_DLL
$(objdir)\$(library_prefix)mpost.exp: $(objdir)\$(library_prefix)mpost.lib

$(objdir)\$(library_prefix)mpost.lib: $(mp_o)
	$(archive) /DEF $(mp_o)

$(objdir)\$(library_prefix)mpost.dll: $(mp_o) $(objdir)\$(library_prefix)mpost.lib $(objdir)\$(library_prefix)mpost.exp $(objdir)\mpost.res $(kpathsealib) $(proglib)
	$(link_dll) $(**) $(conlibs)

$(objdir)\mpost.exe: $(objdir)\win32main.obj $(objdir)\$(library_prefix)mpost.lib $(proglib)
	$(link) $(**) $(conlibs)
!else
$(objdir)\mpost.exe: $(mp_o) $(objdir)\win32main.obj $(objdir)\mpost.res $(kpathsealib) $(proglib)
	$(link) $(**) $(conlibs)
!endif
$(mp_c) mpcoerce.h mpd.h: mp.p $(web2c_texmf) web2c\cvtmf1.sed web2c\cvtmf2.sed
	$(web2c) mp
mp.p mp.pool: $(tangle) mp.web mp-w32.ch
	$(tangle) mp.web mp-w32.ch
mp-w32.ch: $(objdir)\tie.exe mp.ch mp-supp-w32.ch
	$(objdir)\tie.exe -c mp-w32.ch mp.web mp.ch mp-supp-w32.ch
check: mpost-check
mpost-check: mptrap mpost.mem $(mpware)
	.\$(objdir)\mpost --progname=mpost "&./mpost \tracingstats:=1 ; end."
	set MAKEMPX_BINDIR=.;..\contrib\$(objdir)
	set MPXCOMMAND=..\contrib\$(objdir)\makempx.exe
	.\$(objdir)\mpost --progname=mpost $(srcdir)\tests\mptest
	set MAKEMPX_BINDIR=
	set MPXCOMMAND=
	.\$(objdir)\mpost --progname=mpost $(srcdir)\tests\one.two
	.\$(objdir)\mpost --progname=mpost $(srcdir)\tests\uno.dos
clean:: mpost-clean
mpost-clean: mptrap-clean
	-@echo $(verbose) & ( \
		for %%i in ($(mp_o) $(mp_c) mpextra.c mpcoerce.h mpd.h \
                            mp.p mp.pool mpost.mem mpost.log mpout.log \
                            mptest.log one.two.log uno.log) do $(del) %%i $(redir_stderr) \
	)
#	$(LIBTOOL) --mode=clean $(del) mpost

# Can't run trap and mptrap in parallel, because both write trap.{log,tfm}.
mptrap: mpost pltotf tftopl mptrap-clean
mptrap: $(objdir)\mpost.exe $(objdir)\pltotf.exe $(objdir)\tftopl.exe mptrap-clean
	@echo ">>> See $(testdir)\mptrap.diffs for example of acceptable diffs." >&2
	set TEXMFCNFOLD=$(TEXMFCNF)
	set TEXMFCNF=$(testdir)
# get same filename in log 
	-@$(del) mtrap.mp & $(copy) $(testdir)\mtrap.mp . $(redir_stderr)
	.\$(objdir)\pltotf $(testdir)\trapf.pl trapf.tfm
	-.\$(objdir)\mpost -progname=inimpost mtrap
	-$(diff) $(testdir)\mtrap.log mtrap.log
	-$(diff) $(testdir)\mtrap.0 mtrap.0
	-$(diff) $(testdir)\mtrap.1 mtrap.1
	-$(diff) $(testdir)\writeo writeo
	-$(diff) $(testdir)\writeo.2 writeo.2
	-@$(del) trap.mp & $(copy) $(testdir)\trap.mp . $(redir_stderr)
	-@$(del) trap.mpx & $(copy) $(testdir)\trap.mpx . $(redir_stderr)
	-.\$(objdir)\mpost -progname=inimpost<$(testdir)\mptrap1.in >mptrapin.fot
	-$(move) trap.log mptrapin.log
	-$(diff) $(testdir)\mptrapin.log mptrapin.log
# Must run inimp or font_name[null_font] is not initialized, leading to diffs.
	-.\$(objdir)\mpost -progname=inimpost<$(testdir)\mptrap2.in >mptrap.fot
	-$(move) trap.log mptrap.log
	-$(move) trap.tfm mptrap.tfm
	-$(diff) $(testdir)\mptrap.fot mptrap.fot
	-$(diff) $(testdir)\mptrap.log mptrap.log
	-$(diff) $(testdir)\trap.5 trap.5
	-$(diff) $(testdir)\trap.6 trap.6
	-$(diff) $(testdir)\trap.148 trap.148
	-$(diff) $(testdir)\trap.149 trap.149
	-$(diff) $(testdir)\trap.150 trap.150
	-$(diff) $(testdir)\trap.151 trap.151
	-$(diff) $(testdir)\trap.197 trap.197
	-$(diff) $(testdir)\trap.200 trap.200
	.\$(objdir)\tftopl .\mptrap.tfm mptrap.pl
	-$(diff) $(testdir)\mptrap.pl mptrap.pl
	set TEXMFCNF=$(TEXMFCNFOLD)
mptrap-clean:
	-@echo $(verbose) & ( \
		for %%i in (mtrap.mp mtrap.mem trapf.tfm \
			    mtrap.log mtrap.0 mtrap.1 writeo writeo.log writeo.2 \
			    trap.mp trap.mpx mptrapin.fot mptrapin.log \
			    mptrap.fot mptrap.log mptrap.tfm \
			    trap.ps trap.mem trap.0 trap.5 trap.6 trap.95 trap.96 trap.97 \
			    trap.98 trap.99 trap.100 trap.101 trap.102 trap.103 trap.104 \
			    trap.105 trap.106 trap.107 trap.108 trap.109 trap.148 \
			    trap.149 trap.150 trap.151 trap.197 trap.200 \
			    mptrap.pl) do $(del) %%i $(redir_stderr) \
	)
# 
# TeX
tex_c = tex.c
tex_o = $(objdir)\tex.obj
!ifdef TEX_DLL
$(objdir)\$(library_prefix)tex.exp: $(objdir)\$(library_prefix)tex.lib

$(objdir)\$(library_prefix)tex.lib: $(tex_o)
	$(archive) /DEF $(tex_o)

$(objdir)\$(library_prefix)tex.dll: $(tex_o) $(objdir)\$(library_prefix)tex.exp $(objdir)\tex.res $(kpathsealib) $(proglib)
	$(link_dll) $(**) $(socketlibs) $(conlibs)

$(objdir)\tex.exe: $(objdir)\win32main.obj $(objdir)\$(library_prefix)tex.lib $(proglib)
	$(link) $(**) $(conlibs)
!else
$(objdir)\tex.exe: $(tex_o) $(objdir)\win32main.obj $(objdir)\tex.res $(kpathsealib) $(proglib)
	$(link) $(**) $(socketlibs) $(conlibs)
!endif
$(tex_c) texcoerce.h texd.h: tex.p $(web2c_texmf)
	$(web2c) tex
tex.p tex.pool: $(tangle) tex.web tex-w32.ch
	$(tangle) tex.web tex-w32.ch
tex-w32.ch: $(objdir)\tie.exe tex.ch tex-supp-w32.ch
	$(objdir)\tie.exe -c tex-w32.ch tex.web tex.ch tex-supp-w32.ch
check: tex-check
tex-check: trip tex.fmt
	@echo <<.\tex-check.bat
PATH = .\$(objdir);$(kpathseadir)\$(objdir);$(geturldir)\$(objdir);c:\windows\system32;C:\WINDOWS\system32\wbem;c:\windows
rem Test truncation (but don't bother showing the warning msg).
.\$(objdir)\tex --progname=tex --output-comment=$(outcom) $(srcdir)\tests\hello > nul \
  & .\$(objdir)\dvitype hello.dvi | grep olaf@infovore.xs4all.nl > nul
rem \openout should show up in \write's.
.\$(objdir)\tex --progname=tex $(srcdir)\tests\openout & grep xfoo openout.log
rem one.two.tex -> one.two.log
.\$(objdir)\tex --progname=tex $(srcdir)\tests\one.two & dir /n one.two.log
rem uno.dos -> uno.log
.\$(objdir)\tex --progname=tex $(srcdir)\tests\uno.dos & dir /n uno.log
.\$(objdir)\tex --progname=tex $(srcdir)\tests\just.texi & dir /n just.log
.\$(objdir)\tex --progname=tex $(srcdir)\tests\batch.tex
.\$(objdir)\tex --progname=tex --shell $(srcdir)\tests\write18 | grep echo
.\$(objdir)\tex --mltex --progname=initex $(srcdir)\tests\mltextst
.\$(objdir)\tex --progname=tex <nul
set WEB2C=$(kpathsea_srcdir)
set TMPDIR=..
.\$(objdir)\tex --progname=tex "\nonstopmode\font\foo=nonesuch\end"
<<NOKEEP
	.\tex-check.bat

clean:: tex-clean
tex-clean: trip-clean
#	$(LIBTOOL) --mode=clean $(del) tex
	-@echo $(verbose) & ( \
		for %%i in ($(tex_o) $(tex_c) texextra.c texcoerce.h texd.h \
			    tex.p tex.pool tex.fmt tex.log \
			    hello.dvi hello.log xfoo.out openout.log on.two.log uno.log \
			    just.log batch.log write18.log mltextst.log texput.log \
			    missfont.log) do $(del) %%i $(redir_stderr) \
	)
	-@$(deldir) tfm $(redir_stderr)

trip: $(objdir)\dvitype.exe $(objdir)\pltotf.exe $(objdir)\tftopl.exe $(objdir)\tex.exe trip-clean
	@echo ">>> See $(testdir)\trip.diffs for example of acceptable diffs."
	@echo <<.\trip.bat
set TEXMFCNF=$(testdir)
PATH = .\$(objdir);$(kpathseadir)\$(objdir);$(geturldir)\$(objdir);c:\windows\system32;C:\WINDOWS\system32\wbem;c:\windows
.\$(objdir)\pltotf $(testdir)\trip.pl trip.tfm
.\$(objdir)\tftopl .\trip.tfm trip.pl
$(diff) $(testdir)\trip.pl trip.pl
rem get same filename in log 
$(del) trip.tex & $(copy) $(testdir)\trip.tex . $(redir_stderr)
.\$(objdir)\tex -progname=initex < $(testdir)\trip1.in >tripin.fot
$(move) trip.log tripin.log
$(diff) $(testdir)\tripin.log tripin.log
rem May as well test non-ini second time through.
.\$(objdir)\tex < $(testdir)\trip2.in >trip.fot
$(diff) $(testdir)\trip.fot trip.fot
rem We use $(diff) instead of `diff' only for those files where there
rem might actually be legitimate numerical differences.
$(diff) $(diffflags) $(testdir)\trip.log trip.log
.\$(objdir)\dvitype $(dvitype_args) trip.dvi >trip.typ
$(diff) $(diffflags) $(testdir)\trip.typ trip.typ
<<NOKEEP
	.\trip.bat

trip-clean:
	-@echo $(verbose) & ( \
		for %%i in (trip.tfm trip.pl trip.tex trip.fmt tripin.fot tripin.log \
			    trip.fot trip.log trip.dvi trip.typ tripos.tex 8terminal.tex \
			    ) do $(del) %%i $(redir_stderr) \
	)
	-@$(deldir) tfm $(redir_stderr)

# The stub with main() for win32
$(objdir)\win32main.obj: $(objdir) .\lib\win32main.c config.h
	$(compile) -UMAKE_TEX_DLL .\lib\win32main.c

# 
# Makefile fragments:
!include <etexdir/etex.mak>
!include <omegadir$(omegaversion)/omega.mak>
!include <eomegadir/eomega.mak>
!include <alephdir/aleph.mak>
!include <pdftexdir/pdftex.mak>
!include <pdfetexdir/pdfetex.mak>
#!include <pdfxtexdir/pdfxtex.mak>
# 
# Common
programs = $(objdir)\bibtex.exe $(objdir)\ctangle.exe $(objdir)\cweave.exe \
	$(objdir)\dvicopy.exe $(objdir)\dvitomp.exe $(objdir)\dvitype.exe \
	$(objdir)\gftodvi.exe $(objdir)\gftopk.exe $(objdir)\gftype.exe \
	$(objdir)\mft.exe $(odvicopy) $(odvitype) $(otangle) $(objdir)\patgen.exe \
	$(objdir)\pktogf.exe $(objdir)\pktype.exe $(objdir)\pltotf.exe \
	$(objdir)\pooltype.exe $(objdir)\tangle.exe $(objdir)\tftopl.exe \
	$(objdir)\tie.exe $(ttf2afm) $(objdir)\vftovp.exe $(objdir)\vptovf.exe \
	$(objdir)\weave.exe \
	$(pdftosrc) $(ttf2afm) $(tex) \
	$(omega) $(aleph) \
	$(pdfetex) $(pdfxtex) \
	$(mf) $(mfn) $(mpost)
# $(etex) $(eomega) $(pdftex) 

programs: $(objdir) $(programs) $(mpware) $(omegafonts_programs) $(otps_programs) $(pdftosrc)

# We put some programs (written directly in C) in a subdirectory.
$(mpware): $(mpware_sources)
	cd mpware & $(make)

# Additional dependencies for relinking.
$(web2c_programs) $(programs) $(objdir)\tangleboot.exe $(objdir)\ctangleboot.exe: $(kpathsealib) $(proglib)

# 
# Bootstrapping tangle requires making it with itself.  We use the opportunity
# to create an up-to-date tangleboot as well.
$(tangle): $(objdir)\tangle.obj $(objdir)\tangle.res
	$(link) $(objdir)\tangle.obj $(objdir)\tangle.res $(kpathsealib) $(proglib) $(conlibs)
	$(make) tangleboot.p
tangle.c tangle.h: tangle.p
	$(web2c) tangle
# tangle.p is a special case, since it is needed to compile itself.  We
# convert and compile the (distributed) tangleboot.p to make a tangle
# which we use to make the other programs.
tangle.p: $(objdir)\tangleboot.exe tangle.web tangle.ch
	@set WEBINPUTS=$(srcdir)
	.\$(objdir)\tangleboot tangle tangle
	@set WEBINPUTS=
tangle.web:
	@echo $(verbose) & ( \
	  echo "You seem to be missing tangle.web, perhaps because you" & \
	  echo "didn't retrieve web.tar.gz, only web2c.tar.gz." & \
	  echo "You need both." & \
	  echo & \
	  echo "web.tar.gz should be available from the" & \
	  echo "same place that you got web2c.tar.gz." & \
	  echo "In any case, you can get it from" & \
	  echo "ftp://ftp.tug.org/tex/web.tar.gz." & \
	)
	false
clean:: tangle-clean
tangle-clean:
#	$(LIBTOOL) --mode=clean $(del) tangle
	-@echo $(verbose) & ( \
		for %%i in ($(objdir)\tangle.obj tangle.c tangle.h tangle.p) do $(del) %%i $(redir_stderr) \
	)

$(objdir)\tangleboot.exe: $(objdir) $(objdir)\tangleboot.obj
	$(link) $(objdir)\tangleboot.obj $(kpathsealib) $(proglib) $(conlibs)
tangleboot.c tangleboot.h: stamp-tangle $(web2c_programs) $(web2c_common)
	$(web2c) tangleboot
# tangleboot.p is in the distribution
stamp-tangle: tangleboot.p
	date /t >stamp-tangle & time /t >>stamp-tangle
# This is not run unless tangle.web or tangle.ch is changed.
# Only try to run ./tangle if it actually exists, otherwise
# just touch tangleboot.p and build tangle.
tangleboot.p: tangle.web tangle.ch
	set WEBINPUTS=$(srcdir)
	if exist $(objdir)\tangle.exe ( \
	  $(objdir)\tangle tangle tangle \
	  & $(move) tangle.p tangleboot.p \
	  & del cweb.c \
	) else if exist tangleboot.p ( \
	  touch tangleboot.p \
	) \
	else ( \
	  $(copy) $(srcdir)\tangleboot.p . \
	)
	date /t >stamp-ctangle & time /t >>stamp-ctangle
	$(make) $(objdir)\tangle.exe
#	$(objdir)\tangle $(srcdir)\tangle.web $(srcdir)\tangle.ch
#	copy tangle.p tangleboot.p & $(del) tangle.p
#	date /t >stamp-tangle & time /t >>stamp-tangle
#	$(make) $(tangle)
clean:: tangleboot-clean
tangleboot-clean:
#	$(LIBTOOL) --mode=clean $(del) tangleboot
	-@echo $(verbose) & ( \
		for %%i in ($(objdir)\tangleboot.obj tangleboot.c tangleboot.h ) do $(del) %%i $(redir_stderr) \
	)

# Bootstrapping ctangle requires making it with itself.  We use the opportunity
# to create an up-to-date ctangleboot as well.
$(objdir)\ctangle.exe: $(objdir)\ctangle.obj $(objdir)\cweb.obj $(kpathsealib) $(proglib)
	$(link) $(**) $(conlibs)
	$(make) ctangleboot.c 
	$(make) cwebboot.c
# ctangle.c is a special case, since it is needed to compile itself.
ctangle.c: $(objdir)\ctangleboot.exe cwebdir\ctangle.w cwebdir\ctang-w2c.ch
	@set CWEBINPUTS=.;$(srcdir)\cwebdir 
	.\$(objdir)\ctangleboot ctangle.w ctang-w2c.ch
cweb.c: $(objdir)\ctangleboot.exe cwebdir\common.w cwebdir\comm-w2c.ch
	@set CWEBINPUTS=.;$(srcdir)\cwebdir 
	.\$(objdir)\ctangleboot common.w comm-w2c.ch cweb.c
clean:: ctangle-clean
clean:: cweb-clean
ctangle-clean:
#	$(LIBTOOL) --mode=clean $(del) ctangle
	-@echo $(verbose) & ( \
		for %%i in ($(objdir)\ctangle.obj ctangle.c) do $(del) %%i $(redir_stderr) \
	)
cweb-clean:
	-@echo $(verbose) & ( \
		for %%i in ($(objdir)\cweb.obj cweb.c) do $(del) %%i $(redir_stderr) \
	)

$(objdir)\ctangleboot.exe: $(objdir)\ctangleboot.obj $(objdir)\cwebboot.obj $(kpathsealib) $(proglib)
	$(link) $(**) $(conlibs)
# ctangleboot.c is in the distribution
stamp-ctangle: ctangleboot.c cwebboot.c
ctangleboot.c: cwebdir\ctangle.w cwebdir\ctang-w2c.ch
	@set CWEBINPUTS=.;$(srcdir)\cwebdir 
	if exist $(objdir)\ctangle.exe ( \
	  .\$(objdir)\ctangle ctangle.w ctang-w2c.ch \
	  & copy ctangle.c ctangleboot.c \
	  & del ctangle.c \
	) else ( \
	  touch ctangleboot.c \
	)
	date /t >stamp-ctangle & time /t >>stamp-ctangle
	$(make) $(objdir)\ctangle.exe
cwebboot.c: cwebdir\common.w cwebdir\comm-w2c.ch
	set CWEBINPUTS=.;$(srcdir)\cwebdir 
	if exist $(objdir)\ctangle.exe ( \
	  $(objdir)\ctangle common.w comm-w2c.ch cweb.c \
	  & copy cweb.c cwebboot.c \
	  & del cweb.c \
	) else ( \
	  touch cwebboot.c \
	)
	date /t >stamp-ctangle & time /t >>stamp-ctangle
	$(make) $(objdir)\ctangle.exe
clean:: ctangleboot-clean
clean:: cwebboot-clean
ctangleboot-clean:
#	$(LIBTOOL) --mode=clean $(del) ctangleboot
	-@$(del) $(objdir)\ctangleboot.obj $(redir_stderr)
cwebboot-clean:
	-@$(del) $(objdir)\cwebboot.obj $(redir_stderr)

# Even web2c itself uses the library.
# It's annoying to have to give all the filenames here, 
# but texmfmp.c is an exception.
lib_sources = lib\alloca.c lib\basechsuffix.c lib\chartostring.c \
  lib\eofeoln.c lib\fprintreal.c lib\input2int.c lib\inputint.c lib\main.c \
  lib\openclose.c lib\printversion.c lib\uexit.c lib\usage.c lib\version.c \
  lib\zround.c lib\oem.c
$(proglib): $(lib_sources) stamp-auto
	-@echo $(verbose) & ( \
	  pushd lib & $(make) all & popd \
	)

stamp-auto: $(kpathseadir)\..\web2c\c-auto.h

$(kpathseadir)\..\web2c\c-auto.h: c-auto.h

# No exceptions in this library.
window_sources = $(srcdir)\window\*.c
$(windowlib): mfd.h $(window_sources)
	-@echo $(verbose) & ( \
		pushd window & $(make) all & popd \
	)
window\$(objdir)\trap.obj: $(srcdir)\window\trap.c
	-@echo $(verbose) & ( \
		pushd window & $(make) all & popd \
	)
pdflib_sources = $(srcdir)\pdftexdir\*.c $(srcdir)\pdftexdir\*.cc \
	$(srcdir)\pdftexdir\*.h
$(pdflib): $(pdflib_sources)
	-@echo $(verbose) & ( \
		pushd pdftexdir & $(make) all & popd \
	)

# The web2c program consists of several executables.
web2c\$(objdir)\fixwrites.exe: web2c\fixwrites.c
	cd web2c & $(make) all
# web2c\$(objdir)\splitup.exe: web2c\splitup.c
#	 cd web2c & $(make) $(objdir)\splitup.exe
web2c\$(objdir)\web2c.exe: web2c\main.c web2c\web2c.h web2c\web2c.l web2c\web2c.y
	cd web2c & $(make) all

# !include <msvc/tkpathsea.mak>

# 
# Making dumps.
# all_fmts = tex.fmt $(fmts)
# all_formats = $(all_fmts) $(all_efmts) $(all_ofmts) $(all_pdffmts) $(all_pdfefmts) $(all_pdfxfmts)
# all_bases = mf.base $(bases)
# all_mems = mpost.mem $(mems)

dumps:
	@echo Dumps are generated by fmtutil!
#	set TEXMFMAIN
#	..\contrib\$(objdir)\fmtutil --cnffile="$(texmf)\web2c\fmtutil.cnf" --all --dolinks
# fmts: $(all_fmts)
# bases: $(all_bases)
# mems: $(all_mems)
# 
# tex.fmt: $(tex)
# 	$(dumpenv) $(make) progname=tex files="plain.tex cmr10.tfm" prereq-check
# 	$(dumpenv) .\$(objdir)\tex --progname=tex --jobname=tex --ini "\input plain \dump" <nul
# 
# latex.fmt: $(tex)
# 	$(dumpenv) $(make) progname=latex files="latex.ltx" prereq-check
# 	$(dumpenv) .\$(objdir)\tex --progname=latex --jobname=latex --ini "\input latex.ltx" <nul
# 
# olatex.fmt: $(tex)
# 	$(dumpenv) $(make) progname=olatex files="latex.ltx" prereq-check
# 	$(dumpenv) .\$(objdir)\tex --progname=olatex --jobname=olatex --ini "\input latex.ltx" <nul
# 
# mltex.fmt: $(tex)
# 	$(dumpenv) $(make) progname=mltex files="plain.tex cmr10.tfm" prereq-check
# 	$(dumpenv) .\$(objdir)\tex --mltex --progname=mltex --jobname=mltex --ini "\input plain \dump" <nul
# 
# mllatex.fmt: $(tex)
# 	$(dumpenv) $(make) progname=mllatex files="latex.ltx" prereq-check
# 	$(dumpenv) .\$(objdir)\tex --mltex --progname=mllatex --jobname=mllatex --ini "\input latex.ltx" <nul
# 
# mf.base: $(mf)
# 	$(dumpenv) $(make) progname=mf files="plain.mf cmr10.mf $(localmodes).mf" prereq-check
# 	$(dumpenv) .\$(objdir)\mf --progname=mf --jobname=mf --ini "\input plain input $(localmodes) dump" <nul
# 
# mpost.mem: $(mpost)
# 	$(dumpenv) $(make) progname=mpost files=plain.mp prereq-check
# 	$(dumpenv) .\$(objdir)\mpost --progname=mpost --jobname=mpost --ini "\input plain dump" <nul

# This is meant to be called recursively, with $(files) set.
prereq-check: $(kpathseadir)\$(objdir)\kpsewhich.exe
	-$(kpathseadir)\$(objdir)\kpsewhich $(files) > nul
	if ERRORLEVEL 1 $(make) prereq-lose

prereq-lose:
	@echo $(verbose) & ( \
	@echo "You seem to be missing input files necessary to make the" 
	@echo "basic formats (some or all of: $(files))." 
	@echo "Perhaps you've defined the default paths incorrectly, or" 
	@echo "perhaps you have environment variables set pointing" 
	@echo "to an incorrect location.  See ../kpathsea/BUGS." 
	@echo 
	@echo "If you simply do not have the files, you can" 
	@echo "retrieve a minimal set of input files from" 
	@echo "ftp://ftp.tug.org/tex/texklib.tar.gz, mirrored on" 
	@echo "CTAN hosts in systems/web2c." 
	)

$(kpathsea_dir)\$(objdir)\kpsewhich.exe: $(kpathsea)

amstex.fmt: $(tex)
	$(dumpenv) .\$(objdir)\tex.exe --progname=amstex --jobname=amstex --ini amstex.ini <nul

# Texinfo changes the escape character from `\' to `@'.
texinfo.fmt: tex.fmt
	$(dumpenv) .\$(objdir)\tex --progname=texinfo --jobname=texinfo --ini texinfo "@dump" <nul

eplain.fmt: tex.fmt
	touch eplain.aux # Makes cross-reference warnings work right.
	$(dumpenv) .\$(objdir)\tex --progname=eplain --jobname=eplain --ini "&./tex eplain \dump" <nul

# 
!include <msvc/install.mak>

install:: install-exec install-data install-doc
install-exec:: install-links
install-data::
# FIXME!!! I hate to do this, but all this stuff relies more heavily
# on fmtutil than on mere makefiles.
install-dumps:
	@echo <<install-dumps.bat
@echo off
set TEXMFCNF=$(TEXMFCNF)
set TEXMFMAIN=$(TEXMFMAIN)
set TEXMFDIST=$(TEXMFDIST)
set VARTEXMF=$(VARTEXMF)
xcopy /d ..\contrib\$(objdir)\fmtutil.exe $(bindir)\fmtutil.exe
$(bindir)\fmtutil --cnffile="$(texmf)\web2c\fmtutil.cnf" --all --dolinks --force
<<NOKEEP
	-@.\install-dumps.bat

# Installation directories.
$(bindir)::
	@if not exist $(bindir) $(mkdir) $(bindir)
$(texpooldir)::
	@if not exist $(texpooldir) $(mkdir) $(texpooldir)
$(mfpooldir)::
	@if not exist $(mfpooldir) $(mkdir) $(mfpooldir)
$(mppooldir)::
	@if not exist $(mppooldir) $(mkdir) $(mppooldir)
$(fmtdir)::
	@if not exist $(fmtdir) $(mkdir) $(fmtdir)
$(basedir)::
	@if not exist $(basedir) $(mkdir) $(basedir)
$(memdir)::
	@if not exist $(memdir) $(mkdir) $(memdir)
$(web2cdir)::
	@if not exist $(web2cdir) $(mkdir) $(web2cdir)

# The actual binary executables and pool files.
install-programs: $(programs)
	pushd mpware & $(make) install-exec & popd
	-@echo $(verbose) & ( \
	  for %%p in ($(programs)) do $(copy) %%p $(bindir) \
	) $(redir_stdout)

# The links to {mf,mp,tex} for each format and for {ini,vir}{mf,mp,tex},
# plus the equivalents for e-TeX, Omega, and pdf[ex]TeX.
install-links: install-programs install-dumps
#FMU 	-@echo $(verbose) & ( \
#FMU 	    pushd $(bindir) & \
#FMU 	    $(del) .\initex.exe .\virtex.exe & \
#FMU 	    $(lnexe) .\tex.exe $(bindir)\initex.exe & \
#FMU 	    $(lnexe) .\tex.exe $(bindir)\virtex.exe & \
#FMU 	    popd \
#FMU 	) $(redir_stdout)
#FMU 	-@echo $(verbose) & ( \
#FMU 	  pushd $(bindir) & \
#FMU 	    $(del) .\inimf.exe .\virmf.exe & \
#FMU 	    $(lnexe) .\mf.exe $(bindir)\inimf.exe & \
#FMU 	    $(lnexe) .\mf.exe $(bindir)\virmf.exe & \
#FMU #	    $(lnexe) .\mf.exe $(bindir)\mfw.exe & \
#FMU 	  popd \
#FMU 	) $(redir_stdout)
#FMU 	-@echo $(verbose) & ( \
#FMU 	  pushd $(bindir) & \
#FMU 	    $(del) .\inimpost.exe .\virmpost.exe & \
#FMU 	    $(lnexe) .\mpost.exe $(bindir)\inimpost.exe & \
#FMU 	    $(lnexe) .\mpost.exe $(bindir)\virmpost.exe & \
#FMU 	  popd \
#FMU 	) $(redir_stdout)
# 	-@echo $(verbose) & ( \
# 	  if NOT "$(fmts)"=="" \
# 	    for %%i in ($(fmts)) do \
#               pushd $(bindir) & \
# 	        $(del) .\%%~ni.exe & \
# 	        $(lnexe) .\tex.exe $(bindir)\%%~ni.exe & \
# 	      popd \
# 	) $(redir_stdout)
# 	-@echo $(verbose) & ( \
# 	  if not "$(bases)"=="" \
# 	    for %%i in ($(bases)) do \
#               pushd $(bindir) & \
#                 $(del) .\%%~ni.exe & \
# 	        $(lnexe) .\mf.exe $(bindir)\%%~ni.exe & \
# 	      popd \
# 	) $(redir_stdout)
# 	-@echo $(verbose) & ( \
# 	  if not "$(mems)"=="" \
# 	    for %%i in ($(mems)) do \
#               pushd $(bindir) & \
#                 $(del) .\%%~ni.exe & \
# 	        $(lnexe) .\mpost.exe $(bindir)\%%~ni.exe & \
# 	      popd \
# 	) $(redir_stdout)

# Always do plain.*, so examples from the TeXbook (etc.) will work.
# install-fmts: $(all_fmts)
# 	-@echo $(verbose) & ( \
# 	  for %%f in ($(all_fmts)) do $(copy) %%f $(fmtdir)\%%f \
# 	) $(redir_stdout)
# 	-@$(del) -f $(fmtdir)\plain.fmt & $(LN) tex.fmt $(fmtdir)\plain.fmt $(redir_stderr)
# 
# install-bases: $(all_bases)
# 	-@echo $(verbose) & ( \
# 	  for %%f in ($(all_bases)) do $(copy) %%f $(basedir)\%%f \
# 	) $(redir_stdout)
# 	-@$(del) -f $(basedir)\plain.base & $(LN) mf.base $(basedir)\plain.base $(redir_stderr)
# 
# install-mems: $(all_mems)
# 	-@echo $(verbose) & ( \
# 	  for %%f in ($(all_mems)) do $(copy) %%f $(memdir)\%%f \
# 	) $(redir_stdout)
# 	-@$(del) -f $(memdir)\plain.mem & $(LN) mpost.mem $(memdir)\plain.mem $(redir_stderr)

# Auxiliary files.
install-data:: $(texpooldir) $(mfpooldir) $(mppooldir)
	@$(copy) tex.pool $(texpooldir)\tex.pool $(redir_stdout)
	@$(copy) mf.pool $(mfpooldir)\mf.pool $(redir_stdout)
	@$(copy) mp.pool $(mppooldir)\mp.pool $(redir_stdout)
	cd doc & $(make) $(install_makeargs) install-data
	cd man & $(make) $(install_makeargs) install-data
	@$(copy) $(srcdir)\tiedir\tie.1 $(man1dir)\tie.$(manext) $(redir_stdout)
	@$(copy) $(srcdir)\cwebdir\cweb.1 $(man1dir)\cweb.$(manext) $(redir_stdout)
#	@$(copy) fmtutil.cnf $(web2cdir)\fmtutil.cnf $(redir_stdout)

install-doc::
	-@echo $(verbose) & ( \
	  for %d in (doc man) do \
	    echo Entering %d for install & \
	    pushd %d & $(make) install & popd \
	)

# The distribution comes with up-to-date .info* files,
# so this should never be used unless something goes wrong
# with the unpacking, or you modify the manual.
doc\web2c.info:
	cd doc & $(make) info
info dvi:
	cd doc & $(make) $@

# Manual pages
manpages:
	cd man & $(make) all

# !include <msvc/dist.mak>

!include <msvc/config.mak>

# 

installcheck:
	pushd $(srcdir)\tests &  bibtex allbib & popd
	mf "\mode:=ljfour; input logo10" & tftopl logo10.tfm >nul
	tex "\nonstopmode \tracingstats=1 \input story \bye"
# 
# Cleaning.
all_subdirs = doc lib man mpware web2c window $(omegafonts) $(otps) pdftexdir # pdfetexdir pdfxtexdir

# Having a multiple-target rule with the subdir loop fails because of
# the dependencies introduced by clean.mk.  Yet, we want the
# dependencies here at the top level so that distclean will run the
# clean rules, etc.  So, sigh, put the subdir loop in each target and
# only run it if we have a Makefile.  Alternatively, we could do as
# Automake does.

mostlyclean:: tangleboot-clean ctangleboot-clean
	-@echo $(verbose) & ( \
	  for %d in ($(all_subdirs)) do \
	    echo Entering %d for $@ & \
	    pushd %d & $(make) $@ & popd \
	)
clean::
	-@echo $(verbose) & ( \
	  for %d in ($(all_subdirs)) do \
	    echo Entering %d for $@ & \
            pushd %d & $(make) $@ & popd \
	)
	-@echo $(verbose) & ( \
		for %%i in (*.log *.fmt *.efmt *.oft *.eoft *.base *.mem *.fls *.ofl *.aux \
                            mf-w32.ch mp-w32.ch tex-w32.ch) do $(del) %%i $(redir_stderr) \
	)
distclean::
	-@echo $(verbose) & ( \
	  for %d in ($(all_subdirs)) do \
	    echo Entering %d for $@ & \
            pushd %d & $(make) $@ & popd \
	)
	-@$(del) fmtutil.cnf $(redir_stderr)
extraclean::
	-@echo $(verbose) & ( \
	  for %d in ($(all_subdirs)) do \
	    echo Entering %d for $@ & \
	    pushd %d & $(make) $@ & popd \
	)
maintainer-clean::
	-@echo $(verbose) & ( \
	  for %d in ($(all_subdirs)) do \
	    echo Entering %d for $@ & \
	    pushd %d & $(make) $@ & popd \
	)

!include <msvc/clean.mak>

depend::
	-@echo $(verbose) & ( \
	  for %%d in (lib mpware web2c window $(otps)) do \
	    pushd %%d & $(make) depend & popd \
	)

!include <msvc/rdepend.mak>
!include "./depend.mak"

#  
# Local variables:
# page-delimiter: "^# \f"
# mode: Makefile
# End:
