################################################################################
#
# Makefile  : DTL
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/01/21 08:46:39 popineau>
#
################################################################################
root_srcdir = ..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

USE_KPATHSEA = 1
USE_GNUW32 = 1

!include <msvc/common.mak>

DEFS = $(DEFS) -DHAVE_CONFIG_H

programs = $(objdir)\dt2dv.exe $(objdir)\dv2dt.exe
objects = $(objdir)\dt2dv.obj $(objdir)\dv2dt.obj
manfiles = dt2dv.1 dv2dt.1
docfiles = README dtl.doc dvi.doc

default: all

all: $(objdir) $(programs)

$(objdir)\dv2dt.exe: $(objdir)\dv2dt.obj $(kpathsealib)
	$(link) $(**) $(conlibs)

$(objdir)\dt2dv.exe: $(objdir)\dt2dv.obj $(kpathsealib)
	$(link) $(**) $(conlibs)

test: hello example tripvdu edited

hello:  hello.dtl $(programs)
	$(objdir)\dt2dv hello.dtl hello2.dvi
	$(objdir)\dv2dt hello2.dvi hello2.dtl
	-@diff hello.dtl hello2.dtl > hello.dif
	type hello.dif
#	@if [ -s hello.dif ] ; \
#	then echo ERROR: differences in hello.dif ; \
#	else $(RM) hello.dif ; \
#	fi

hello.dtl:  hello.tex
	tex hello
	$(objdir)\dv2dt hello.dvi hello.dtl

example:  example.dtl $(programs)
	$(objdir)\dt2dv example.dtl example2.dvi
	$(objdir)\dv2dt example2.dvi example2.dtl
	-@diff example.dtl example2.dtl > example.dif
	type example.dif
#	@if [ -s example.dif ] ; \
#	then echo ERROR: differences in example.dif ; \
#	else $(RM) example.dif ; \
#	fi

example.dtl:  example.tex
	tex example
	$(objdir)\dv2dt example.dvi example.dtl

tripvdu:  tripvdu.dtl $(programs)
	$(objdir)\dt2dv tripvdu.dtl tripvdu2.dvi
	$(objdir)\dv2dt tripvdu2.dvi tripvdu2.dtl
	-@diff tripvdu.dtl tripvdu2.dtl > tripvdu.dif
	type tripvdu.dif
#	@if [ -s tripvdu.dif ] ; \
#	then echo ERROR: differences in tripvdu.dif ; \
#	else $(RM) tripvdu.dif ; \
#	fi

tripvdu.dtl:  tripvdu.tex
	tex tripvdu
	$(objdir)\dv2dt tripvdu.dvi tripvdu.dtl

# edited.txt is already a dtl file.

edited:  edited.txt $(programs)
	$(objdir)\dt2dv edited.txt edited.dvi
	$(objdir)\dv2dt edited.dvi edited2.dtl
	-@diff edited.txt edited2.dtl > edited.dif
	$(objdir)\dt2dv edited2.dtl edited2.dvi
	$(objdir)\dv2dt edited2.dvi edited3.dtl
	-@diff edited2.dtl edited3.dtl >> edited.dif
	type edited.dif
#	@if [ -s edited.dif ] ; \
#	then echo ERROR : differences in edited.dif ; \
#	else $(RM) edited.dif ; \
#	fi

!include <msvc/config.mak>

!include <msvc/install.mak>

install:: install-exec install-man

!include <msvc/clean.mak>

clean::
	-@$(del) *.log *.dvi *.dtl *.dif

!include <msvc/rdepend.mak>
!include "./depend.mak"

#
# Local Variables:
# mode: makefile
# End:# End: