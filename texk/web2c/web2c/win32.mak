################################################################################
#
# Makefile  : Web2C / Web2C converter
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <04/01/07 11:12:14 popineau>
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

!include <msvc/common.mak>

LEX_OUTPUT_ROOT = lex_yy

DEFS = -I.. $(DEFS) -DHAVE_CONFIG_H -DOEM

proglib = ..\lib\$(objdir)\lib.lib
programs = $(objdir)\web2c.exe	\
	$(objdir)\fixwrites.exe	\
#	$(objdir)\splitup.exe

web2c_objects = \
	$(objdir)\y_tab.obj			\
	$(objdir)\$(LEX_OUTPUT_ROOT).obj	\
	$(objdir)\main.obj

lib_sources = \
	..\lib\basechsuffix.c	\
	..\lib\oem.c		\
	..\lib\chartostring.c	\
	..\lib\coredump.c	\
	..\lib\eofeoln.c	\
	..\lib\fprintreal.c	\
	..\lib\inputint.c	\
	..\lib\input2int.c	\
	..\lib\main.c		\
	..\lib\openclose.c	\
	..\lib\printversion.c	\
	..\lib\uexit.c		\
	..\lib\usage.c		\
	..\lib\version.c	\
	..\lib\zround.c		\
	..\c-auto.h

default: all

all: $(objdir) $(programs)

$(objdir)\web2c.exe: $(web2c_objects) $(objdir)\web2c.res $(kpathsealib) $(proglib)
	$(link) $(**) $(conlibs) 

# We use y_tab.* for the sake of MS-DOS.
$(objdir)\main.obj: main.c y_tab.h

y_tab.c y_tab.h: web2c.y
	@echo Expect one shift/reduce conflict.
#	@set BISON_SIMPLE=$(gnushare)/bison.simple
	$(yacc) -d -v web2c.y -o y_tab.c

$(objdir)\$(LEX_OUTPUT_ROOT).obj: y_tab.h

$(LEX_OUTPUT_ROOT).c: web2c.l
	$(lex) -t web2c.l | $(sed) "/^extern int isatty YY/d" > $(LEX_OUTPUT_ROOT).c

$(objdir)\fixwrites.exe: $(objdir)\fixwrites.obj $(kpathsealib) $(proglib)
	$(link) $(**) $(conlibs)

$(objdir)\splitup.exe: $(objdir)\splitup.obj $(kpathsealib) $(proglib)
	$(link) $(**) $(conlibs)

$(objdir)\regfix.exe: $(objdir)\regfix.obj $(kpathsealib) $(proglib)
	$(link) $(**) $(conlibs)

$(proglib): $(lib_sources)
	pushd ..\lib & $(make) all & popd

!include <msvc/config.mak>
!include <msvc/install.mak>
!include <msvc/clean.mak>

clean::
	-@echo $(verbose) & ( \
		for %%i in (y_tab.c y_tab.h y.output yacc.* $(LEX_OUTPUT_ROOT).c) do $(del) %%i $(redir_stderr) \
	)

!include <msvc/rdepend.mak>
!include "./depend.mak"

# Local Variables:
# mode: Makefile
# End:
