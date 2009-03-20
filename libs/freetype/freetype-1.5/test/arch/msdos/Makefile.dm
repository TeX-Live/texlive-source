# This file is part of the FreeType project.
#
# It builds the library and test programs for emx-gcc and djgpp under MSDOS.
#
# You will need dmake.
#
# Use this file while in the 'test' directory with the following statement:
#
#   dmake -r -f arch/msdos/Makefile.dm
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

ARCH = arch/msdos
FT_MAKEFILE = $(ARCH)/Makefile.dm
FT_MAKE = dmake -r

.IMPORT: COMSPEC
SHELL := $(COMSPEC)
SHELLFLAGS := /c
GROUPSHELL := $(SHELL)
GROUPFLAGS := $(SHELLFLAGS)
GROUPSUFFIX := .bat
SHELLMETAS := *"?<>&|

CC = gcc

LIBDIR = ../lib
LIBDIR_DOS = ..\lib
INCDIRS = -I$(LIBDIR) -I$(LIBDIR)/$(ARCH) -I. -I$(LIBDIR)/extend

CFLAGS = -Wall -ansi -O2 -g $(INCDIRS)
# CFLAGS = -ansi -Wall -O2 -s $(INCDIRS)

# full-screen MSDOS driver
GDRIVER = $(ARCH)/gfs_dos.c

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

GSRC = gmain.c display.c blitter.c $(GDRIVER)
GOBJ = $(GSRC:.c=.o)


%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.exe:
	$(CC) $(CFLAGS) -o $@ @$(mktmp $(&:t"\n")\n)


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

.PHONY: all debug freetype freetype_debug \
        clean distclean do_clean do_distclean depend


all: freetype $(EXEFILES)

debug: freetype_debug $(EXEFILES)

freetype:
[
	cd $(LIBDIR_DOS)
	$(FT_MAKE) -f $(FT_MAKEFILE) all
	cd $(MAKEDIR)
]

freetype_debug:
[
	cd $(LIBDIR_DOS)
	$(FT_MAKE) -f $(FT_MAKEFILE) debug
	cd $(MAKEDIR)
]

ftzoom.exe: $(GOBJ) ftzoom.o common.o $(LIBDIR)/libttf.a
ftview.exe: $(GOBJ) ftview.o common.o $(LIBDIR)/libttf.a
ftlint.exe: ftlint.o common.o $(LIBDIR)/libttf.a
ftdump.exe: ftdump.o common.o $(LIBDIR)/libttf.a
ftstring.exe: $(GOBJ) ftstring.o common.o $(LIBDIR)/libttf.a
fttimer.exe: $(GOBJ) fttimer.o common.o $(LIBDIR)/libttf.a
ftstrpnm.exe: ftstrpnm.o common.o $(LIBDIR)/libttf.a
ftsbit.exe: ftsbit.o common.o textdisp.o $(LIBDIR)/libttf.a
ftmetric.exe: ftmetric.o common.o textdisp.o $(LIBDIR)/libttf.a
ftstrtto.exe: $(GOBJ) ftstrtto.o common.o arabic.o $(LIBDIR)/libttf.a


clean: do_clean
[
	cd $(LIBDIR_DOS)
	$(FT_MAKE) -f $(FT_MAKEFILE) clean
	cd $(MAKEDIR)
]

distclean: do_clean do_distclean
[
	cd $(LIBDIR_DOS)
	$(FT_MAKE) -f $(FT_MAKEFILE) distclean
	cd $(MAKEDIR)
]

do_distclean:
-[
	del dep.end
	del *.exe
	del core
]

do_clean:
-[
	del *.o
	del $(ARCH)\gfs_dos.o
]

#depend: $(SRC) $(GSRC)
#[
#	cd $(LIBDIR_DOS)
#	$(FT_MAKE) -f $(FT_MAKEFILE) depend
#	cd $(MAKEDIR)
#	$(CC) -E -M $(INCDIRS) @$(mktmp $(<:t"\n")\n) > dep.end
#]
#
#ifeq (dep.end,$(wildcard dep.end))
#  include dep.end
#endif

# end of Makefile.dm
