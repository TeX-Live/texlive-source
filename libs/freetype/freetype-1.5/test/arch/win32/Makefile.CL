# This file is part of the FreeType project.
#
# It builds the library and test programs for Microsoft Visual C++.
#
# You will need NMAKE.
#
#
# Use this file while in the 'test' directory with the following statement:
#
#   nmake /f arch\win32\Makefile.CL
#
# A DLL version of the library can be built and then used with
#
#   nmake DLL=1 /f arch\win32\Makefile.CL dll
#
# (do not forget to define DLL, otherwise the link phase will fail).
#
#
# Debug versions can be obtained with
#
#   nmake DEBUG=1 /f arch\win32\Makefile.CL
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

ARCH = arch\win32
FT_MAKEFILE = $(ARCH)\Makefile.CL
FT_MAKE = $(MAKE) /nologo
FT_DLL = ft15_32.dll

CC = cl /nologo

LIBDIR = ..\lib
INCDIRS = -I$(LIBDIR) -I$(LIBDIR)\$(ARCH) -I. -I$(LIBDIR)\extend

CFLAGS_ANSI = /Za
!ifndef DEBUG
CFLAGS =     /Ox /W2 $(INCDIRS)
LDFLAGS = 
!else
CFLAGS = /Zi /Ge /W2 $(INCDIRS)
LDFLAGS = /Zi
!endif

!ifdef DLL
CFLAGS = $(CFLAGS) /DEXPORT_DEF=__declspec(dllexport) /DFREETYPE_DLL
!endif


# Windows graphic driver
GDRIVER = $(ARCH)\gw_win32.c

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
	         $(CFLAGS) $(CFLAGS_ANSI) $<
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

dll: the_dll $(EXEFILES)

!else
# Skipped if non-DEBUG build
default_target: debug
dll: the_debug_dll $(EXEFILES)

!endif

debug: freetype_debug $(EXEFILES)

freetype:
	cd $(LIBDIR)
	$(FT_MAKE) /f $(FT_MAKEFILE) all
	cd ..\test

freetype_debug:
	cd $(LIBDIR)
	$(FT_MAKE) /f $(FT_MAKEFILE) DEBUG=1 debug
	cd ..\test

the_dll:
	cd $(LIBDIR)
	$(FT_MAKE) /f $(FT_MAKEFILE) DLL=1 dll
	cd ..\test
    -copy $(LIBDIR)\$(FT_DLL)

the_debug_dll:
	cd $(LIBDIR)
	$(FT_MAKE) /f $(FT_MAKEFILE) DEBUG=1 DLL=1 dll
	cd ..\test
    -copy $(LIBDIR)\$(FT_DLL)


# C compilers are unable to include 32-bit <windows.h> in ANSI mode.
# So we have a special rule for this file, to build it outside ANSI.
$(GDRIVER:.c=.obj):
	$(CC) /c /Fo$* @<<
	    $(CFLAGS) /Ze $(GDRIVER)
<<

.obj.exe:
	$(CC) /Fe$* @<<
	    $(LDFLAGS) $** GDI32.LIB USER32.LIB
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
	-del *.dll
	-del *.pdb

do_clean:
	-del *.obj
	-del $(ARCH)\*.obj
	-del *.ilk
	-del *.pch
	-del *.exp


!include "$(ARCH)\depend.win"

# end of Makefile.CL
