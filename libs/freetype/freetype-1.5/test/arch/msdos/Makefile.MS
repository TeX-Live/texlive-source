# This file is part of the FreeType project.
#
# It builds the library and test programs for Microsoft C for MS-DOS,
# large model. It also works for Visual C++ 1.x 16-bits compiler, but
# you should instead use the Makefile customized for it, Makefile.VC.
#
# You will need NMAKE.
#
#
# Use this file while in the 'test' directory with the following statement:
#
#   nmake /f arch\msdos\Makefile.MS
#
#
# Debug versions can be obtained with
#
#   nmake DEBUG=1 /f arch\msdos\Makefile.MS
#
# A special version enabled to handle big fonts (with more than 16,384
# glyphs) can be obtained with
#
#   nmake BIGFONTS=1 /f arch\msdos\Makefile.MS
#
#
# Copyright 1996-2001 by
# David Turner, Robert Wilhelm, and Werner Lemberg.
#
# This file is part of the FreeType project, and may only be used, modified,
# and distributed under the terms of the FreeType project license,
# LICENSE.TXT.  By continuing to use, modify, or distribute this file you
# indicate that you have read the license and understand and accept it
# fully.

ARCH = arch\msdos
FT_MAKEFILE = $(ARCH)\Makefile.MS
FT_MAKE = $(MAKE) /nologo

CC = cl /nologo

LIBDIR = ..\lib
INCDIRS = -I$(LIBDIR) -I$(LIBDIR)\$(ARCH) -I. -I$(LIBDIR)\extend

!ifndef DEBUG
CFLAGS = /Ox /AL /Za /W2 /G2 $(INCDIRS)
LDFLAGS = /AL
!else
CFLAGS = /Zi /AL /Za /W2 /G2 $(INCDIRS)
LDFLAGS = /Zi /AL
!endif


# full-screen MSDOS driver
GDRIVER = $(ARCH)\gfs_dos.c

GSRC = display.c gmain.c blitter.c $(GDRIVER)

GOBJ = $(GSRC:.c=.obj)

SRC = arabic.c \
      common.c \
      textdisp.c \
      ftdump.c \
      fterror.c \
      ftlint.c \
      ftmetric.c \
      ftsbit.c \
      ftstring.c \
      ftstrpnm.c \
      ftstrtto.c \
      fttimer.c \
      ftview.c \
      ftzoom.c

OBJ = $(SRC:.c=.obj)


.c.obj:
	@$(CC) /c /Fo$* @<<
	    $(CFLAGS) $<
<<

EXEFILES = ftdump.exe \
           fterror.exe \
           ftlint.exe \
           ftmetric.exe \
           ftsbit.exe \
           ftstring.exe \
           ftstrpnm.exe \
           ftstrtto.exe \
           fttimer.exe \
           ftview.exe \
           ftzoom.exe

!ifndef DEBUG
# Skiped if DEBUG build
all: freetype $(EXEFILES)

!endif

debug: freetype_debug $(EXEFILES)

!ifdef BIGFONTS
MAKEBIG = BIGFONTS=1
!endif

freetype:
	cd $(LIBDIR)
	$(FT_MAKE) /f $(FT_MAKEFILE) $(MAKEBIG) all
	cd ..\test

freetype_debug:
	cd $(LIBDIR)
	$(FT_MAKE) /f $(FT_MAKEFILE) DEBUG=1 $(MAKEBIG) debug
	cd ..\test

.obj.exe:
	$(CC) /Fe$* @<<
	      $(LDFLAGS) $**
<<

ftzoom.exe: $(GOBJ) ftzoom.obj common.obj $(LIBDIR)\libttf.lib
ftview.exe: $(GOBJ) ftview.obj common.obj $(LIBDIR)\libttf.lib
ftstring.exe: $(GOBJ) ftstring.obj common.obj $(LIBDIR)\libttf.lib
ftstrtto.exe: $(GOBJ) ftstrtto.obj common.obj arabic.obj $(LIBDIR)\libttf.lib
fttimer.exe: $(GOBJ) fttimer.obj common.obj $(LIBDIR)\libttf.lib
ftlint.exe: ftlint.obj common.obj $(LIBDIR)\libttf.lib
ftdump.exe: ftdump.obj common.obj $(LIBDIR)\libttf.lib
ftstrpnm.exe: ftstrpnm.obj common.obj $(LIBDIR)\libttf.lib
ftsbit.exe: ftsbit.obj common.obj textdisp.obj $(LIBDIR)\libttf.lib
ftmetric.exe: ftmetric.obj common.obj textdisp.obj $(LIBDIR)\libttf.lib
fterror.exe: fterror.obj common.obj $(LIBDIR)\libttf.lib


clean: do_clean
	cd $(LIBDIR)
	$(FT_MAKE) /f $(FT_MAKEFILE) clean
	cd ..\test

distclean: do_clean
	cd $(LIBDIR)
	$(FT_MAKE) /f $(FT_MAKEFILE) distclean
	cd ..\test
	-del *.exe

do_clean:
	-del *.obj
	-del $(ARCH)\*.obj


!include "$(ARCH)\depend.dos"

# end of Makefile.MS
