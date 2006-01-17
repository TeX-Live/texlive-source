################################################################################
#
# Makefile  : TeXk / contrib
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <03/09/18 11:26:01 popineau>
#
################################################################################
root_srcdir=..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

USE_KPATHSEA=1
USE_MKTEX=1
MAKE_MKTEX = 1
USE_REGEX=1
USE_GNUW32=1

# FIXME: delay load kpathsea for irun !!!

!include <msvc/common.mak>

DEFS = $(DEFS) -DHAVE_CONFIG_H -DKPATHSEA_DLL=\"$(kpathseaname).dll\"

mktex_objs = $(objdir)\fileutils.obj	\
	$(objdir)\mktex.obj		\
	$(objdir)\variables.obj 	\
	$(objdir)\stackenv.obj

mktex_progs = $(objdir)\mktexlsr.exe	\
	$(objdir)\mktexnam.exe		\
	$(objdir)\mktexupd.exe		\
	$(objdir)\mktexpk.exe		\
	$(objdir)\mktexdir.exe		\
	$(objdir)\mktexmf.exe		\
	$(objdir)\mktextfm.exe		\
#	$(objdir)\mkofm.exe		\
#	$(objdir)\mktextex.exe

programs = \
	$(mktexdll)			\
	$(objdir)\dvihp.exe		\
	$(objdir)\makempx.exe		\
	$(objdir)\mktex.exe		\
	$(objdir)\fmtutil.exe		\
	$(objdir)\mkocp.exe		\
	$(objdir)\mkofm.exe		\
	$(objdir)\irun.exe		\
	$(objdir)\pdfdde.exe

pdfddechilds = $(objdir)\pdfopen.exe $(objdir)\pdfclose.exe

libfiles = $(mktexlib)

default: all

all: $(objdir) $(mktex) $(programs)

lib: $(objdir) $(mktexlib)

!ifdef MKTEX_DLL
DEFS = -DMAKE_MKTEX_DLL $(DEFS) 

$(mktexlib): $(mktex_objs) 
	$(archive) /DEF $(mktex_objs)

$(mktexdll): $(mktex_objs) $(objdir)\libmktex.res $(regexlib) $(kpathsealib)
	$(link_dll) $(**) $(mktexlib:.lib=.exp) $(conlibs)
!else
$(mktexlib): $(mktex_objs) $(regexlib)
	$(archive) $(**)
!endif

mktex_progs: $(objdir)\mktex.exe
	for %%i in ($(mktex_progs)) \
		do $(copy) $(objdir)\mktex.exe $(objdir)\%%~nxi

$(objdir)\makempx.exe: $(objdir)\makempx.obj $(mktexlib) $(objdir)\makempx.res $(kpathsealib)
	$(link) $(**) $(conlibs)

$(objdir)\fmtutil.exe: $(objdir)\fmtutil.obj $(mktexlib) $(kpathsealib)
	$(link) $(**) $(conlibs)

$(objdir)\dvihp.exe: $(objdir)\dvihp.obj $(mktexlib) $(objdir)\dvihp.res $(kpathsealib)
	$(link) $(**) $(conlibs)

$(objdir)\mktex.exe: $(objdir)\main.obj $(mktexlib) $(kpathsealib)
	$(link) $(**) $(conlibs)

$(objdir)\mkocp.exe: $(objdir)\mkocp.obj $(mktexlib) $(kpathsealib)
	$(link) $(**) $(conlibs)

$(objdir)\mkofm.exe: $(objdir)\mkofm.obj $(gnuw32lib)
	$(link) $(**) $(conlibs)

$(objdir)\makempx.obj:	makempx.c
	$(compile) -UMAKE_MKTEX_DLL makempx.c

$(objdir)\fmtutil.obj:	fmtutil.c
	$(compile) -UMAKE_MKTEX_DLL fmtutil.c

$(objdir)\main.obj: main.c
	$(compile) -UMAKE_MKTEX_DLL main.c

$(objdir)\dvihp.obj: dvihp.c
	$(compile) -UMAKE_MKTEX_DLL dvihp.c

$(objdir)\mkocp.obj: mkocp.c
	$(compile) -UMAKE_MKTEX_DLL mkocp.c

$(objdir)\mkofm.obj: mkofm.c
	$(compile) -UKPSE_DLL -UMAKE_MKTEX_DLL mkofm.c

$(objdir)\irun-tmp.obj: $(srcdir)\irun.c 
	echo define RDATA 0x3D000 | sed "s/define/#define/" > $(srcdir)\irun.h
	$(compile) $(srcdir)\irun.c

$(objdir)\irun-tmp.exe: $(objdir)\irun-tmp.obj  $(kpathsealib)
	$(link) $(**) $(conlibs)

$(objdir)\irun.obj: $(srcdir)\irun.c $(objdir)\irun-tmp.exe
	dumpbin /headers $(objdir)\irun-tmp.exe \
	| sed -n -e "/^[ \t]*FOOBAR/,/^[ \t]*FOOBAR/p" \
	| sed -n -e "/file pointer/p" \
	| sed -e "/raw data/s/^[ \t]*\([0-9A-Z]*\).*$$/#define RDATA 0x\1/" \
	| sed -e "/file pointer/d" \
	> $(srcdir)\irun.h
	$(compile) $(srcdir)\irun.c
#	-$(RM) $(objdir)\irun-tmp.*

$(objdir)\irun.exe: $(objdir)\irun.obj $(objdir)\irun.res $(kpathsealib)
	$(link) $(**) $(conlibs) $(kpathsealib)

$(objdir)\pdfdde.exe: $(objdir)\pdfdde.obj $(gnuw32lib)
	$(link) $(**) $(conlibs) shell32.lib

$(objdir)\pdfdde.obj: pdfdde.c
	$(compile) -UKPSE_DLL pdfdde.c

!include <msvc/config.mak>
!include <msvc/install.mak>

install:: install-exec

install-exec::
	-@echo $(verbose) & ( \
		echo "Installing mktex program files in $(MAKEDIR)" & \
		( for %%i in ($(mktex_progs)) do \
			$(copy) $(objdir)\mktex.exe $(bindir)\%%~nxi $(redir_stdout) ) & \
		$(copy) $(objdir)\fmtutil.exe $(bindir)\mktexfmt.exe $(redir_stdout) \
	)
	-@echo $(verbose) & ( \
	for %%i in ($(pdfddechilds)) \
		do $(copy) $(objdir)\pdfdde.exe $(bindir)\%%~nxi $(redir_stdout) )

!include <msvc/clean.mak>

!include <msvc/rdepend.mak>
!include "./depend.mak"

#Local Variables:
#mode: Makefile
#End:
