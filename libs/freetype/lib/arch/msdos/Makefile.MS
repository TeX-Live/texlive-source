# This file is part of the FreeType project.
#
# It builds the library for Microsoft C for MS-DOS, large model.
# It also works for Visual C++ 1.x 16-bits compiler, but you should
# instead use the Makefile customized for it, Makefile.VC.
# Due to size constraints, it does not try to pack all modules into one.
#
# Tested with Microsoft C v.6.0ax, C/C++ v.7.0, Visual C++ v.1.5.
# Microsoft C v.5.1 chokes on some (definitively) ANSI constructs,
# like switch() with an expression of type long. Upgrade your compiler!
#
# You will need NMAKE.
#
# Use this file while in the lib directory with the following statement:
#
#   nmake /f arch\msdos\Makefile.MS
#
#
# A debug version can be obtained with
#
#   nmake DEBUG=1 /f arch\msdos\Makefile.MS
#
# A special version enabled to handle big fonts (with more than 16,384
# glyphs) can be obtained with
#
#   nmake BIGFONTS=1 /f arch\msdos\Makefile.MS
#
#
# Copyright 1996-2002 by
# David Turner, Robert Wilhelm, and Werner Lemberg.
#
# This file is part of the FreeType project, and may only be used, modified,
# and distributed under the terms of the FreeType project license,
# LICENSE.TXT.  By continuing to use, modify, or distribute this file you
# indicate that you have read the license and understand and accept it
# fully.

ARCH = arch\msdos
FT_MAKEFILE = $(ARCH)\Makefile.MS

CC = cl /nologo
LIB = lib

!ifndef DEBUG
CFLAGS = /Ox /AL /W0 /G2 /I$(ARCH) /I. /Iextend
!else
CFLAGS = /Zi /AL /W0 /G2 /I$(ARCH) /I. /Iextend
!endif


!ifndef BIGFONTS
CFLAGS = $(CFLAGS) /Za

TTFILE = .\ttfile.c
TTMEMORY = .\ttmemory.c
!else
CFLAGS = $(CFLAGS) /DTT_HUGE_PTR=huge /Ze

TTFILE = $(ARCH)\hugefile.c
TTMEMORY = $(ARCH)\hugemem.c
!endif

TTMUTEX = .\ttmutex.c

PORT = $(TTFILE) $(TTMEMORY) $(TTMUTEX)

OBJS_X = extend\ftxgasp.obj extend\ftxkern.obj  extend\ftxpost.obj \
         extend\ftxcmap.obj extend\ftxwidth.obj extend\ftxerr18.obj \
         extend\ftxsbit.obj extend\ftxopen.obj  extend\ftxgsub.obj \
         extend\ftxgpos.obj extend\ftxgdef.obj
# the line below does not work with these old versions of nmake...
# OBJS_X = $(SRC_X:.c=.obj)

OBJS_M = ttapi.obj     ttcache.obj   ttcalc.obj   ttcmap.obj  ttdebug.obj \
         ttgload.obj   ttinterp.obj  ttload.obj   ttobjs.obj  \
         ttraster.obj  ttextend.obj  $(PORT)      $(OBJS_X)
# OBJS_M = $(SRC_M:.c=.obj) $(OBJS_X)

# Not used here because the compiler runs out of memory...
OBJ_S = $(ARCH)\freetype.obj
#SRC_S = $(ARCH)\freetype.c
OBJS_S = $(OBJ_S) $(OBJS_X)


# Since Microsoft's NMAKE does not handle $($(LIB_FILES)), and using
# LIB_FILES="$(OBJS_S)" will excess the capacity of COMMAND.COM, we include
# each .obj in the library as soon as it is compiled. Better schemes are
# used in the other Makefile's, please give them a look.
.c.obj:
	$(CC) /c /Fo$@ $(CFLAGS) $*.c
	$(LIB) libttf -+$*.obj;


!ifndef DEBUG
# Skipped if DEBUG build
#   (but it changes nothing, since we always build in multiple parts).
all: $(OBJS_M)
#	-del libttf.lib
#	$(LIB) libttf.lib @<<response
#+ $(OBJS_M: = -+ );
#<<KEEP
!endif

debug: $(OBJS_M)
#	-del libttf.lib
#	$(LIB) libttf.lib @<<response
#+ $(OBJS_M: = -+ );
#<<KEEP

#$(OBJ_S): $(SRC_S) $(SRC_M)

# Not used here because it excesses the capacity of COMMAND.COM...
libttf.lib: $(LIB_FILES)
	$(LIB) $@ +-$(?: =-+);

clean:
	-del *.obj
	-del extend\*.obj
	-del $(ARCH)\*.obj
	-del response

distclean: clean
	-del libttf.lib

!include "$(ARCH)\depend.dos"

# end of Makefile.MS
