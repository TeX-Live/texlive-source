################################################################################
#
# Makefile  : Dvipsk
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <03/11/16 17:29:08 popineau>
#
################################################################################
root_srcdir = ..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

version = 5.86g

# Add -DDEFRES=dpi to DEFS to change the default resolution from 600.
# Add -DSECURE if you will install dvips with special privileges.
# Add -DNO_DEBUG to omit debugging support.
# Add -DNO_EMTEX to omit EMTEX specials.
# Add -DNO_HPS to omit hypertex support.
# Add -DNO_TPIC to omit tpic support.
# 
# For VM/CMS or (perhaps) VMS or DOS compilation, need the corresponding
# subdirectory from the original dvips distribution.  (Maybe dvipsk
# won't work at all on such systems; if you have info one way or the
# other, please let kb@cs.umb.edu know.)
USE_KPATHSEA = 1
USE_GNUW32 = 1

!include <msvc/common.mak>

DEFS = $(DEFS) -DHAVE_CONFIG_H

!ifdef USE_KPATHSEA
DEFS = $(DEFS) -DDEFRES=600 -DA4 -DSHIFTLOWCHARS \
	-DKPATHSEA                               \
	-DNeedFunctionPrototypes

!else
DEFS = $(DEFS) -DDEFRES=600
!endif

# writet1.c is taken from pdftex
pdftexdir = ..\web2c\pdftexdir

# The `t[mp]-' business is so that we don't create an empty .pro file,
# if running squeeze fails for some reason.  A dependency on squeeze
# fails here, so we include it below.
.SUFFIXES: .pro .lpro
.lpro.pro:
	.\$(objdir)\squeeze <$< >t-$@
	$(copy) t-$@ $@ && $(del) t-$@ $(redir_stdout)

prologues = tex.pro texps.pro texc.pro special.pro finclude.pro \
            color.pro crop.pro hps.pro

srcfiles = dospecial.c dviinput.c emspecial.c fontdef.c loadfont.c \
	dvips.c tfmload.c download.c prescan.c scanpage.c          \
	skippage.c output.c scalewidth.c dosection.c dopage.c      \
	resident.c search.c unpack.c drawPS.c header.c repack.c  \
	virtualfont.c dpicheck.c finclude.c $(pdftexdir)\writet1.c pprescan.c    \
	papersiz.c color.c bbox.c hps.c flib.c

objects = $(objdir)\dospecial.obj $(objdir)\dviinput.obj   \
	$(objdir)\emspecial.obj $(objdir)\fontdef.obj      \
	$(objdir)\loadfont.obj $(objdir)\dvips.obj         \
	$(objdir)\tfmload.obj $(objdir)\download.obj       \
	$(objdir)\prescan.obj $(objdir)\scanpage.obj       \
	$(objdir)\skippage.obj $(objdir)\output.obj        \
	$(objdir)\scalewidth.obj $(objdir)\dosection.obj   \
	$(objdir)\dopage.obj $(objdir)\resident.obj        \
	$(objdir)\search.obj $(objdir)\unpack.obj          \
	$(objdir)\drawPS.obj $(objdir)\header.obj          \
	$(objdir)\repack.obj $(objdir)\virtualfont.obj     \
	$(objdir)\dpicheck.obj $(objdir)\finclude.obj      \
	$(objdir)\writet1.obj $(objdir)\pprescan.obj       \
	$(objdir)\papersiz.obj $(objdir)\color.obj         \
	$(objdir)\bbox.obj $(objdir)\hps.obj

program = $(objdir)\dvips.exe
programs = $(objdir)\afm2tfm.exe $(program)

installdirs =  $(mandir) $(infodir) $(psheaderdir)/base # $(dvips_plain_macrodir)

manfiles = dvips.1 afm2tfm.1
infofiles = dvips.info # dvips.info-1 dvips.info-2 dvips.info-3 dvips.info-3 dvips.info-4
pdfdocfiles = dvips.pdf
docsubdir = dvips

default all: $(objdir) $(programs) $(prologues) dvips.info

!ifdef USE_KPATHSEA
$(program): $(kpathsealib) $(objects) $(objdir)\dvips.res
	$(link) $(**) $(conlibs)
$(objdir)\afm2tfm.exe: $(kpathsealib) $(objdir)\afm2tfm.obj $(objdir)\afm2tfm.res
	$(link) $(**) $(conlibs)
# Omit LOADLIBES since no need to dynamically link with kpathsea.
$(objdir)\squeeze.exe: $(objdir)\squeeze.obj
	$(link) $(**) $(conlibs)

$(objdir)\writet1.obj: $(pdftexdir)\writet1.c
	$(compile) -I$(pdftexdir) $(pdftexdir)\writet1.c

!else
# where TeX is `rooted'.  Sometimes /usr/local/lib/tex.
# TEXDIR = /usr/lib/tex
TEXDIR = e:/Local/TeX/share/texmf

# another place pk, tfm, and vf files might be found.
# LOCALDIR = /LocalLibrary/Fonts/TeXFonts
LOCALDIR = .

# the default path to search for TFM files 
# (this usually is identical to TeX's defaultfontpath, which omits `.')
# (private fonts are given an explicit directory, which overrides the path)
#   overridden by the environment variable TEXFONTS
# TFMPATH = $(LOCALDIR)/tfm;$(TEXDIR)/fonts/tfm
TFMPATH = .;$(LOCALDIR)/tfm;$(TEXDIR)/fonts/tfm

# the default path to search for PK files (usually omits `.')
# Don't forget to add the directory that
# MakeTeXPK puts the files!  (In this case, /LocalLibrary/Fonts...)
#   overridden by the environment variable TEXPKS or TEXPACKED or PKFONTS
# PKPATH = $(LOCALDIR)/pk;$(TEXDIR)/fonts/pk
PKPATH = .;$(LOCALDIR)/pk;$(TEXDIR)/fonts/pk

# the default path to search for VF files (usually omits `.')
#   overridden by the environment variable VFFONTS
# VFPATH = $(LOCALDIR)/vf;$(TEXDIR)/fonts/vf
VFPATH = .;$(LOCALDIR)/vf;$(TEXDIR)/fonts/vf

# additional directories in which to search for subdirectories to find
# both tfm and pk files
FONTSUBDIRPATH = 

# where the config files go
# CONFIGDIR = $(TEXDIR)/ps
CONFIGDIR = $(TEXDIR)/dvips

# the default path to search for config files
#   overridden by the environment variable TEXCONFIG
# CONFIGPATH = .;$(CONFIGDIR)
CONFIGPATH = .;$(CONFIGDIR)

# the name of your config file
# CONFIGFILE = config.ps
CONFIGFILE = config.ps

# where the header PS files go
# HEADERDIR = $(TEXDIR)/ps
HEADERDIR = $(TEXDIR)/dvips

# the default path to search for header files
# HEADERPATH = .;$(HEADERDIR)
HEADERPATH = .;$(HEADERDIR)

# where epsf.tex and rotate.tex go (usually the TeX macros directory)
# TEXMACRODIR = $(TEXDIR)/inputs
TEXMACRODIR = $(TEXDIR)/inputs

# the default path to search for epsf and psfiles
# (usually the same as TeX's defaultinputpath)
# FIGPATH = .;..;$(TEXDIR)/inputs
FIGPATH = .;..;$(TEXDIR)/inputs

PATHS = -DTFMPATH=\"$(TFMPATH)\" \
	-DPKPATH=\"$(PKPATH)\" \
	-DVFPATH=\"$(VFPATH)\" \
	-DHEADERPATH=\"$(HEADERPATH)\" \
	-DCONFIGPATH=\"$(CONFIGPATH)\" \
        -DCONFIGFILE=\"$(CONFIGFILE)\" \
	-DFONTSUBDIRPATH=\"$(FONTSUBDIRPATH)\" \
	-DFIGPATH=\"$(FIGPATH)\"

DEFS = $(DEFS) $(PATHS)

objects = $(objects) $(objdir)/makefont.obj

$(program): $(objects) $(objdir)\dvips.res
	$(link) $(**) $(conlibs)
$(objdir)\afm2tfm.exe: $(objdir)\afm2tfm.obj $(objdir)\afm2tfm.res
	$(link) $(**) $(conlibs)
# Omit LOADLIBES since no need to dynamically link with kpathsea.
$(objdir)\squeeze.exe: $(objdir)\squeeze.obj
	$(link) $(**) $(conlibs)
!endif

$(prologues): $(objdir)\squeeze.exe

texc.lpro: texc.script tex.lpro
	$(sed) -f $(win32seddir)/texc-script.sed < .\tex.lpro > $@

!include <msvc/config.mak>

!include <msvc/install.mak>

check: all
	-@$(del) mtest.ps
	set TFMFONTS=$(srcdir)/testdata
	set VFFONTS=$(srcdir)/testdata
	-.\$(objdir)\dvips -D 300 $(srcdir)/testdata/dvipstst.xdv -o dvipstst.ps
	$(move) dvipstst.ps mtest.ps
	-.\$(objdir)\dvips -D 300 $(srcdir)/testdata/dvipstst.xdv -o
	-diff --ignore-all-space $(srcdir)/testdata/dvipstst.xps dvipstst.ps
	-@$(del) dvipstst.ps # mtest.ps

install:: install-exec install-data install-info install-man install-doc
	@echo "Create $(psheaderdir)/base/config.ps if necessary (see ./INSTALL)."

install-data::
# PostScript prologues and encodings.
	-@echo $(verbose) & ( \
		for %%h in ($(prologues)) do $(copy) %%h $(psheaderdir)\base\%%h $(redir_stdout) \
	)
	-@echo $(verbose) & ( \
		for %%e in (*.enc) do $(copy) reencode\%%e $(psheaderdir)\base\%%e $(redir_stdout) \
	)
!ifdef TETEX
	-@echo $(verbose) & ( \
		for %%m in (blackdvi.tex colordvi.tex epsf.tex rotate.tex) do \
			$(copy) tex\%%m $(dvips_plain_macrodir)\%%m $(redir_stdout) \
	)
!endif

!include <msvc/clean.mak>

extraclean::
	-@$(del) afm2tfm.ps dvips.ps testdata\*.log testdata\dvipstst.dvi \
	testdata\dvipstst.pdf testdata\dvipstst.ps testdata\mtest.ps

distclean::
	-@$(del) *.pro texc.lpro

!include <msvc/rdepend.mak>
!include "./depend.mak"

#
# Local Variables:
# mode: Makefile
# End:
