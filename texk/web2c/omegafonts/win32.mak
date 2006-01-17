################################################################################
#
# Makefile  : Web2C / Omega / Omegafonts
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <04/01/07 11:10:43 popineau>
#
################################################################################
root_srcdir = ..\..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

USE_KPATHSEA = 1
USE_GNUW32 = 1

# These get expanded from the parent directory, not this one.
!include <msvc/common.mak>

DEFS = -I.. $(DEFS) -DHAVE_CONFIG_H

LEX_OUTPUT_ROOT = lex_yy

proglib = ..\lib\$(objdir)\lib.lib

programs = $(objdir)\omfonts.exe
linked_programs = $(objdir)\ofm2opl.exe $(objdir)\opl2ofm.exe \
	$(objdir)\ovf2ovp.exe $(objdir)\ovp2ovf.exe

omfonts_objects = $(objdir)\omfonts.obj $(objdir)\y_tab.obj 			\
		  $(objdir)\$(LEX_OUTPUT_ROOT).obj $(objdir)\char_routines.obj	\
		  $(objdir)\dimen_routines.obj $(objdir)\error_routines.obj 	\
                  $(objdir)\header_routines.obj $(objdir)\ligkern_routines.obj	\
		  $(objdir)\list_routines.obj $(objdir)\out_routines.obj	\
		  $(objdir)\param_routines.obj $(objdir)\print_routines.obj	\
                  $(objdir)\font_routines.obj $(objdir)\extra_routines.obj 	\
		  $(objdir)\out_ofm.obj $(objdir)\parse_ofm.obj

all: $(objdir) $(programs)

$(objdir)\omfonts.exe: $(omfonts_objects) $(proglib) $(kpathsealib)
	$(link) $(**) $(conlibs)

$(objdir)\omfonts.obj: omfonts.c y_tab.h

y_tab.c y_tab.h: pl.y
	$(yacc) -d -v $(srcdir)\pl.y -o y_tab.c

$(objdir)\$(LEX_OUTPUT_ROOT).obj: parser.h

$(LEX_OUTPUT_ROOT).c: pl.l
	$(lex) -t $(srcdir)\pl.l | sed "/^extern int isatty YY/d" > $(LEX_OUTPUT_ROOT).c

!include <msvc/config.mak>
!include <msvc/install.mak>

install:: install-exec

install-programs: install-exec

install-exec:: install-links

install-links: $(programs)
	-@echo $(verbose) & for %%i in ($(linked_programs)) do $(copy) $(programs) $(bindir)\%%~nxi $(redir_stdout)

!include <msvc/clean.mak>

clean::
	-@echo $(verbose) & ( \
		for %%i in (y_tab.c y_tab.h y.output yacc.* $(LEX_OUTPUT_ROOT).c) do $(del) %%i $(redir_stderr) \
	)

!include <msvc/rdepend.mak>
!include "./depend.mak"

#
# Local Variables:
# mode: makefile
# End:
