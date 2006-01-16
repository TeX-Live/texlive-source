################################################################################
#
# Makefile  : Unzip
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/09/23 17:41:22 popineau>
#
################################################################################
root_srcdir=..\..
INCLUDE=$(INCLUDE);$(root_srcdir)\texk

USE_GNUW32 = 1
USE_UNZIP = 1
MAKE_UNZIP = 1
USE_ADVAPI = 1

!include <msvc/common.mak>

{win32}.c{$(objdir)}.obj:
	$(compile) $<
{windll}.c{$(objdir)}.obj:
	$(compile) $<
{windll}.rc{$(objdir)}.res:
	$(RC) $(rcvars) $(rcflags) /fo$@ $<

DEFS= $(DEFS) -DHAVE_CONFIG_H -DWINDLL -DUSE_EF_UT_TIME -DDLL -DUNZIPLIB

programs = $(unzipdll)
libfiles = $(unziplib)
includefiles = unzip.h
objects = \
	$(objdir)\api.obj	\
	$(objdir)\crc32.obj	\
	$(objdir)\crctab.obj	\
	$(objdir)\crypt.obj	\
	$(objdir)\explode.obj	\
	$(objdir)\extract.obj	\
	$(objdir)\fileio.obj	\
	$(objdir)\globals.obj	\
	$(objdir)\inflate.obj	\
	$(objdir)\list.obj	\
	$(objdir)\match.obj	\
	$(objdir)\nt.obj	\
	$(objdir)\process.obj	\
	$(objdir)\ttyio.obj	\
	$(objdir)\unreduce.obj	\
	$(objdir)\unshrink.obj	\
	$(objdir)\win32.obj	\
	$(objdir)\windll.obj	\
	$(objdir)\zipinfo.obj

default: all

all: $(objdir) $(unzip)

lib: $(objdir) $(unziplib)

!ifdef UNZIP_DLL
DEFS = $(DEFS) -DMAKE_UNZIP_DLL

$(unziplib): $(objdir) $(objects)
	$(archive) /DEF $(objects)

$(unzipdll): $(objects) $(objdir)\windll.res
	$(link_dll) $(**) $(unziplib:.lib=.exp) $(conlibs)
!else
$(unziplib): $(objdir) $(objects)
	$(archive) $(objects)
!endif

!include <msvc/config.mak>
!include <msvc/install.mak>

install:: install-exec install-include install-lib

!include <msvc/clean.mak>
!include <msvc/rdepend.mak>
!include "./depend.mak"

#
# Local Variables:
# mode: makefile
# End: