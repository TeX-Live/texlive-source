################################################################################
#
# Makefile  : Zlib
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <03/07/06 03:01:05 popineau>
#
################################################################################
root_srcdir=..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

USE_ZLIB = 1
MAKE_ZLIB = 1
USE_GNUW32 = 1

!include <msvc/common.mak>

programs = $(zlibdll)
libfiles = $(zliblib)
includefiles = zlib.h zconf.h
manfiles = zlib.3
objects = $(objdir)\adler32.obj	\
	$(objdir)\compress.obj	\
	$(objdir)\crc32.obj	\
	$(objdir)\gzio.obj	\
	$(objdir)\uncompr.obj	\
	$(objdir)\deflate.obj	\
	$(objdir)\trees.obj	\
	$(objdir)\zutil.obj	\
	$(objdir)\inflate.obj	\
	$(objdir)\infblock.obj	\
	$(objdir)\inftrees.obj	\
	$(objdir)\infcodes.obj	\
	$(objdir)\infutil.obj	\
	$(objdir)\inffast.obj

TEST_OBJS = $(objdir)\example.obj $(objdir)\minigzip.obj

DISTFILES = README INDEX ChangeLog configure Make*[a-z0-9] descrip.mms \
	    zlib.def zlib.rc algorithm.doc  *.[ch]

default: all

all: $(objdir) $(zlib) $(programs)

lib: $(objdir) $(zlib)

!ifdef ZLIB_DLL
DEFS = $(DEFS) -DMAKE_ZLIB_DLL

zlibdef = $(library_prefix)zlib.def

$(zlibdef): zlib.def
	sed -e "/^NAME/s/^.*$$/NAME $(library_prefix)zlib.dll/" < $(**) > $@

$(zliblib): $(zlibdef) $(objects)
	$(archive) /DEF $(objects)

$(zlibdll): $(objects) $(objdir)\zlib.res $(gnuw32lib)
	$(link_dll) $(objects) $(zliblib:.lib=.exp) $(objdir)\zlib.res $(conlibs) $(gnuw32lib)
!else
$(zliblib): $(objects)
	$(archive) $(objects)
!endif

test: all $(objdir)\example.exe $(objdir)\minigzip.exe
	.\$(objdir)\example.exe
	echo hello world | .\$(objdir)\minigzip | .\$(objdir)\minigzip -d 

$(objdir)\example.exe: $(objdir)\example.obj $(zliblib) $(gnuw32lib)
	$(link) $(objdir)\example.obj $(zliblib) $(conlibs) $(gnuw32lib)

$(objdir)\minigzip.exe: $(objdir)\minigzip.obj $(zliblib) $(gnuw32lib)
	$(link) $(objdir)\minigzip.obj $(zliblib) $(conlibs) $(gnuw32lib)

!include <msvc/config.mak>
!include <msvc/install.mak>

install:: install-exec install-lib install-include

!include <msvc/clean.mak>

clean::
	-@$(del) $(objdir)\minigzip.exe foo.gz

!include <msvc/rdepend.mak>
!include "./depend.mak"

#
# Local Variables:
# mode: makefile
# End:
