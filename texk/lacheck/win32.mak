################################################################################
#
# Makefile  : Lacheck
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <03/06/29 11:22:39 popineau>
#
################################################################################
root_srcdir = ..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

REV = 1.25
version = 1.0

USE_GNUW32 = 1

!include <msvc/common.mak>

# LEX = flex -8
LEXLIB = 
LEX_OUTPUT_ROOT = lex_yy

prog_cflags = -DNeedFunctionPrototypes

program = $(objdir)\lacheck.exe
objects = $(objdir)\$(LEX_OUTPUT_ROOT).obj

manfiles = lacheck.1

programs = $(program)

default: all

all: $(objdir) $(programs)

$(program): $(objects)  $(gnuw32lib)
	$(link) $(**) $(conlibs)

!include <msvc/config.mak>

!include <msvc/install.mak>

install:: install-exec install-man

lacheck.1: 	lacheck.man
	-$(sed) -e "s!%%LACHECKREV%%!Release $(REV)!" $(srcdir)/lacheck.man | \
	    	$(sed) -e "s!%%LACHECKPATH%%!$(bindir:\=/)/lacheck!" > $@ 

!include <msvc/clean.mak>

clean::
	-@$(del) $(LEX_OUTPUT_ROOT).c 

$(LEX_OUTPUT_ROOT).c: lacheck.lex
	$(lex) -t lacheck.lex | $(sed) "/^extern int isatty YY/d" > $(LEX_OUTPUT_ROOT).c

$(objdir)\$(LEX_OUTPUT_ROOT).obj: $(LEX_OUTPUT_ROOT).c

!include <msvc/rdepend.mak>
!include "./depend.mak"

#
# Local Variables:
# mode: makefile
# End:

