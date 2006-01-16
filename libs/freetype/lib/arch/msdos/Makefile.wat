# This file is part of the FreeType project
#
# This builds the Watcom library with Watcom's wcc386 under DOS
#
# You'll need Watcom's wmake
#
#
# Invoke by "wmake -f arch\msdos\Makefile.wat" while in the "lib" directory
#
# This will build "freetype\lib\libttf.lib"
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
# receive notices of successfull builts since many years.  So be prepared
# to some problems; and if it works for you, or if you can upgrade the
# package in any way, please notify us!


ARCH = arch\msdos
FT_MAKEFILE = $(ARCH)\Makefile.wat
FT_MAKE = wmake -h


.EXTENSIONS:
.EXTENSIONS: .lib .obj .c .h
.obj:.;.\extend;.\$(ARCH)
.c:.;.\extend;.\$(ARCH)
.h:.;.\extend;.\$(ARCH)

CC = wcc386

CCFLAGS = /otexanl+ /s /w4 /zq /d3 -I$(ARCH) -I. -Iextend


# FIXME: should use something like OBJ = $(SRC:.c=.obj)

SRC_X = ftxgasp.c ftxkern.c ftxpost.c &
        ftxcmap.c ftxwidth.c ftxsbit.c ftxerr18.c &
        ftxgsub.c ftxgpos.c ftxopen.c ftxgdef.c
OBJS_X = ftxgasp.obj ftxkern.obj ftxpost.obj &
         ftxcmap.obj ftxwidth.obj ftxsbit.obj ftxerr18.obj &
         ftxgsub.obj ftxgpos.obj ftxopen.obj ftxgdef.obj

SRC_M = ttapi.c ttcache.c ttcalc.c ttcmap.c ttdebug.c &
        ttfile.c ttgload.c ttinterp.c &
        ttload.c ttmemory.c ttmutex.c ttobjs.c ttraster.c &
        ttextend.c
OBJS_M = ttapi.obj ttcache.obj ttcalc.obj ttcmap.obj ttdebug.obj &
        ttfile.obj ttgload.obj ttinterp.obj &
        ttload.obj ttmemory.obj ttmutex.obj ttobjs.obj ttraster.obj &
        ttextend.obj $(OBJS_X)

SRC_S = freetype.c
OBJ_S = freetype.obj
OBJS_S = $(OBJ_S) $(OBJ_X)


.c.obj:
  $(CC) $(CCFLAGS) $[* /fo=$[*.obj

libname = libttf
libfile = $(libname).lib
cmdfile = $(libname).lst


all: .symbolic
  $(FT_MAKE) -f $(FT_MAKEFILE) LIB_FILES=OBJS_S $(libfile)

debug: .symbolic
  $(FT_MAKE) -f $(FT_MAKEFILE) LIB_FILES=OBJS_M $(libfile)


$(libfile): $($(LIB_FILES))
  wlib -q -n $(libfile) @$(cmdfile)

# is this correct? Know nothing about wmake and the Watcom compiler...
$(OBJ_S): $(SRC_S) $(SRC_M)
  $(CC) $(CCFLAGS) $(SRC_S) /fo=$(OBJ_S)

$(cmdfile): $($(LIB_FILES))
  @for %i in ($($(LIB_FILES))) do @%append $(cmdfile) +-%i

clean: .symbolic
  @-erase $(OBJ_S)
  @-erase $(OBJS_M)
  @-erase $(cmdfile)

distclean: .symbolic clean
  @-erase $(libfile)

new: .symbolic
  @-wtouch *.c

# end of Makefile.wat
