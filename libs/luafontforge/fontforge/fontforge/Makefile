
top_srcdir = ../../../../src/libs/luafontforge/fontforge
srcdir = $(top_srcdir)/fontforge

CC = gcc
AR = ar cru
RANLIB = ranlib

OBJECTS =autohint.o charview.o fontinfo.o \
 fontview.o fvfonts.o parsepfa.o parsettf.o prefs.o psread.o namelist.o \
 splinefill.o splineoverlap.o splinestroke.o splineutil.o splineutil2.o stamp.o \
 start.o encoding.o macbinary.o splineorder2.o kernclass.o ttfspecial.o \
 parsettfatt.o contextchain.o macenc.o splinerefigure.o parsettfvar.o pua.o \
 stemdb.o lookups.o featurefile.o \
 tottf.o tottfgpos.o tottfvar.o splinesave.o

_CFLAGS = -I$(top_srcdir)/inc -I$(srcdir) -I. -Wall $(XCFLAGS) \
 -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 \
 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 \
 -DHAVE_INTTYPES_H=1 -DHAVE_UNISTD_H=1 -D_NO_PYTHON=1 -DHAVE_LANGINFO_H=1 \
 -DX_DISPLAY_MISSING=1 -DHAVE_LONG_DOUBLE=1 -D_HAS_LONGLONG=1 \
 -DNODYNAMIC=1 -D_NO_FREETYPE=1 -D_NO_LIBUNINAMESLIST=1 \
 -DFONTFORGE_CONFIG_NO_WINDOWING_UI=1 -DLUA_FF_LIB=1 -UHAVE_LIBINTL_H

CFLAGS = -g -O2 $(_CFLAGS)

all: $(OBJECTS)

# The slight error introduced occasionally by optimization turns out to have
# disasterous effects. This file may not be compiled with the optimizer.
# On the other hand we do need CPPFLAGS to get the right includes...
splinerefigure.o: $(srcdir)/splinerefigure.c $(srcdir)/splinefont.h
	$(CC) -g -c -o splinerefigure.o $(_CFLAGS) $(srcdir)/splinerefigure.c

%.o: $(srcdir)/%.c $(srcdir)/splinefont.h $(srcdir)/views.h $(srcdir)/stemdb.h $(srcdir)/ttf.h $(srcdir)/unicoderange.h $(srcdir)/psfont.h $(srcdir)/groups.h $(srcdir)/edgelist.h $(srcdir)/edgelist2.h
	$(CC) $(CFLAGS) -c $< -o $@
