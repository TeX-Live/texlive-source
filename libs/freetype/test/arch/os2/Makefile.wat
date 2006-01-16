# This file is part of the FreeType project
#
# This builds the test programs with the Watcom compiler
#
# You'll need Watcom's wmake
#
# Invoke by "wmake -f arch\os2\Makefile.wat" when in the "test" directory
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
#
# This compiler did compile early versions of the FreeType package.
# While we tried our best to maintain the makefiles, the development team
# does not have currently access to a copy of this compiler, and we did not
# receive notices of successful builts since many years.  So be prepared
# to have some problems; and if it works for you, or if you can upgrade the
# package in any way, please notify us!

ARCH = arch\os2
FT_MAKEFILE = $(ARCH)\Makefile.wat
FT_MAKE = wmake -h

.EXTENSIONS:
.EXTENSIONS: .exe .obj .c .h
.obj:.
.c:.
.h:.;..\lib

CC = wcl386

LIBDIR  = ..\lib
INCDIRS = -I$(LIBDIR) -I$(LIBDIR)\$(ARCH) -I$(LIBDIR)\extend
LIBFILE = $(LIBDIR)\libttf.lib

LINK_OPTS = 

OBJ_CFLAGS = /c /otexanl+ /s /w4 /zq $(INCDIRS)

CCFLAGS = /otexanl+ /s /w4 /zq $(INCDIRS)

GFSDRIVER = $(ARCH)\gfs_os2.obj
GFSDRIVER_SRC = $(ARCH)\gfs_os2.c

GPMDRIVER = $(ARCH)\gpm_os2.obj
GPMDRIVER_SRC = $(ARCH)\gpm_os2.c
GPMDRIVER_DEF = $(ARCH)\gpm_os2.def

SRC = gmain.c display.c blitter.c textdisp.c &
      fttimer.c ftview.c ftlint.c ftzoom.c ftdump.c ftstring.c &
      ftstrpnm.c ftsbit.c ftmetric.c ftstrtto.c &
      $(GPMDRIVER_SRC) $(GFSDRIVER_SRC)

GFSOBJ = gmain.obj $(GFSDRIVER) blitter.obj display.obj
GPMOBJ = gmain.obj $(GPMDRIVER) blitter.obj display.obj

PM = $(LIBFILE) $(GPMOBJ) common.obj
FS = $(LIBFILE) $(GFSOBJ) common.obj


# graphics utility and test driver

EXEFILES = ftview.exe ftviewfs.exe &
           fttimer.exe fttimefs.exe &
           ftlint.exe &
           ftdump.exe &
           ftstring.exe ftstrfs.exe &
           ftzoom.exe ftzoomfs.exe &
           ftstrpnm.exe &
           ftsbit.exe &
           ftmetric.exe &
           ftstrtto.exe ftstrtfs.exe


all: freetype $(EXEFILES)

debug: freetype_debug $(EXEFILES)


freetype: .symbolic
  cd ..\lib
  $(FT_MAKE) -f $(FT_MAKEFILE) all
  cd ..\test

freetype_debug: .symbolic
  cd ..\lib
  $(FT_MAKE) -f $(FT_MAKEFILE) debug
  cd ..\test

# implicit rules
#
.c.obj :
  $(CC) $(OBJ_CFLAGS) $[* /fo=$[*.obj


# the full-screen graphics driver
#
$(GFSDRIVER): $(GFSDRIVER_SRC)
    $(CC) $(OBJ_CFLAGS) $[*.c /fo=$[*.obj

# the pm graphics driver
#
$(GPMDRIVER): $(GPMDRIVER_SRC)
  $(CC) $(OBJ_CFLAGS) $[*.c /fo=$[*.obj

ftzoom.exe : ftzoom.obj $(LIBFILE) $(PM) $(GPMDRIVER_DEF)
  $(CC) $(CCFLAGS) -l=os2v2_pm $(PM) $[*.c /fe=$[*.exe

ftzoomfs.exe : ftzoom.obj $(LIBFILE) $(FS)
  $(CC) $(CCFLAGS) $(FS) $[@ /fe=ftzoomfs.exe

ftview.exe : ftview.obj $(LIBFILE) $(PM) $(GPMDRIVER_DEF)
  $(CC) $(CCFLAGS) -l=os2v2_pm $(PM) $[*.c /fe=$[*.exe

ftviewfs.exe : ftview.obj $(LIBFILE) $(FS)
  $(CC) $(CCFLAGS) $(FS) $[*.c /fe=ftviewfs.exe

ftstring.exe : ftstring.obj $(LIBFILE) $(PM) $(GPMDRIVER_DEF)
  $(CC) $(CCFLAGS) -l=os2v2_pm $(PM) $[*.c /fe=$[*.exe 

ftstrfs.exe : ftstring.obj $(LIBFILE) $(FS)
  $(CC) $(CCFLAGS) $(FS) $[*.c /fe=ftstrfs.exe

fttimer.exe: fttimer.obj $(LIBFILE) $(PM) $(GPMDRIVER_DEF)
  $(CC) $(CCFLAGS) -l=os2v2_pm $(PM) $[*.c /fe=$[*.exe

fttimefs.exe: fttimer.obj $(LIBFILE) $(FS)
  $(CC) $(CCFLAGS) $(FS) $[*.c /fe=fttimefs.exe

ftlint.exe: ftlint.obj $(LIBFILE)
  $(CC) $(CCFLAGS) $(LIBFILE) common.obj $[*.c /fe=$[*.exe

ftdump.exe: ftdump.obj $(LIBFILE)
  $(CC) $(CCFLAGS) $(LIBFILE) common.obj $[*.c /fe=$[*.exe

ftstrpnm.exe: ftstrpnm.obj $(LIBFILE)
  $(CC) $(CCFLAGS) $(LIBFILE) common.obj $[*.c /fe=$[*.exe

ftsbit.exe: ftsbit.obj textdisp.obj $(LIBFILE)
  $(CC) $(CCFLAGS) $(LIBFILE) common.obj textdisp.obj $[*.c /fe=$[*.exe

ftmetric.exe: ftmetric.obj textdisp.obj $(LIBFILE)
  $(CC) $(CCFLAGS) $(LIBFILE) common.obj textdisp.obj $[*.c /fe=$[*.exe

ftstrtto.exe : ftstrtto.obj arabic.obj $(LIBFILE) $(PM) $(GPMDRIVER_DEF)
  $(CC) $(CCFLAGS) -l=os2v2_pm $(PM) $[*.c /fe=$[*.exe 

ftstrtfs.exe : ftstrtto.obj arabic.obj $(LIBFILE) $(FS)
  $(CC) $(CCFLAGS) $(FS) $[*.c /fe=ftstrtfs.exe


clean: .symbolic
  @-erase *.obj
  @-erase $(ARCH)\*.obj

distclean: .symbolic  clean
  @-erase *.exe
  @-erase *.err
  cd ..\lib
  $(FT_MAKE) -f $(FT_MAKEFILE) distclean
  cd ..\test

new: .symbolic
  @-wtouch *.c

# end of Makefile.wat
