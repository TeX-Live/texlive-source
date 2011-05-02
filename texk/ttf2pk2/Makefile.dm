# Makefile for ttf2pk -- loyer@enst.fr, wl@gnu.org
#
# This Makefile assumes that you've already built and installed
# the FreeType library.
#
# It builds the ttf2pk and ttf2tfm for emx-gcc.
#
# You will need dmake.
#
# Use this file while with the following statement:
#
#   dmake -r -f Makefile.dm


.IMPORT: COMSPEC
SHELL := $(COMSPEC)
SHELLFLAGS := /c
GROUPSHELL := $(SHELL)
GROUPFLAGS := $(SHELLFLAGS)
GROUPSUFFIX := .bat
SHELLMETAS := *"?<>&|

CC = gcc
LIBDIR = ../../libs/freetype/lib
INCDIR = -I$(LIBDIR) -I.

# CFLAGS = -Wall -O2 -g $(INCDIR) -fbounds-checking -DHAVE_EMTEXDIR -DMSDOS
CFLAGS = -Wall -O2 -s $(INCDIR) -DHAVE_EMTEXDIR -DMSDOS

SRC = case.c emdir.c emtexdir.c errormsg.c filesrch.c ligkern.c newobj.c \
      parse.c pklib.c subfont.c texenc.c tfmaux.c ttf2pk.c ttf2tfm.c \
      ttfaux.c ttfenc.c ttflib.c vplaux.c

ttf2pkobjs = emdir.o emtexdir.o errormsg.o filesrch.o ligkern.o newobj.o \
             parse.o pklib.o subfont.o texenc.o ttf2pk.o ttfenc.o ttflib.o
ttf2tfmobjs = case.o emdir.o emtexdir.o errormsg.o filesrch.o ligkern.o \
              newobj.o parse.o subfont.o texenc.o tfmaux.o ttf2tfm.o \
              ttfaux.o ttfenc.o vplaux.o


%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.exe:
	$(CC) $(CFLAGS) -o $@ @$(mktmp $(&:t"\n")\n)


PROGRAMS = ttf2pk.exe ttf2tfm.exe

.PHONY: all clean distclean


all: $(PROGRAMS)

ttf2pk.exe: $(ttf2pkobjs) $(LIBDIR)/libttf.a
ttf2tfm.exe: $(ttf2tfmobjs) $(LIBDIR)/libttf.a


clean:
-[
	del *.o
]

distclean: clean
-[
	del dep.end
	del *.exe
	del core
]

#end of Makefile.dm
