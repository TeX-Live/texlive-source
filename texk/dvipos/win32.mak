################################################################################
#
# Makefile  : DVIpos
# Author    : Jin-Hwan Cho <chofchof@ktug.or.kr>
# Platform  : Win32, Microsoft VC++ 7.0, depends upon fpTeX 0.6 sources
# Time-stamp: <03/06/27 00:00:00 ChoF>
#
################################################################################
root_srcdir = ..\..
INCLUDE=$(INCLUDE);$(root_srcdir)\..\win32

version = 20070107

USE_GNUW32 = 1
USE_KPATHSEA = 1

!include <make/common.mak>

DEFS = $(DEFS)

programs=$(objdir)\dvipos.exe

headers = dvicore.h tfm.h utils.h

src = dvicore.c dvipos.c tfm.c utils.c

objects = $(objdir)\dvicore.obj $(objdir)\dvipos.obj $(objdir)\tfm.obj $(objdir)\utils.obj

default: all

all: $(objdir) $(programs)

$(objdir)\dvipos.exe: $(objects) $(kpathsealib)
	$(link) $(**) $(conlibs)

!include <make/install.mak>

install:: install-exec install-data install-doc

distclean::
	@$(copy) config.dvipos config-dvipos $(redir_stdout)

!include <make/clean.mak>

distclean::
	@$(copy) config-dvipos config.dvipos $(redir_stdout)
	-@$(del) config-dvipos $(redir_stderr)

!include <make/rdepend.mak>
!include "./depend.mak"

#  
# Local variables:
# page-delimiter: "^# \f"
# mode: Makefile
# End:
