# This file is part of the FreeType project.
#
# It builds the library and test programs for emx-gcc under OS/2
#
# You will need dmake.
#
# Use this file while in the 'test' directory with the following statement:
#
#   dmake -r -f arch/os2/Makefile.dm
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

ARCH = arch/os2
FT_MAKEFILE = $(ARCH)/Makefile.dm
FT_MAKE = dmake -r

.IMPORT: COMSPEC
SHELL := $(COMSPEC)
SHELLFLAGS := /c
GROUPSHELL := $(SHELL)
GROUPFLAGS := $(SHELLFLAGS)
GROUPSUFFIX := .cmd
SHELLMETAS := *"?<>&|

CC = gcc

LIBDIR = ../lib
LIBDIR_OS2 = $(subst,/,\ $(LIBDIR))
INCDIRS = -I$(LIBDIR) -I$(LIBDIR)/$(ARCH) -I. -I$(LIBDIR)/extend

# CFLAGS = -Wall -ansi -O2 -g $(INCDIRS)
CFLAGS = -ansi -Wall -O2 -s -Zcrtdll $(INCDIRS)

# full-screen MSDOS driver
GFSDRIVER = $(ARCH)/gfs_os2.c
GPMDRIVER = $(ARCH)/gpm_os2.c
GPM_DEF = $(ARCH)/gpm_os2.def

DISPLAY = display.c

SRC = ftlint.c fttimer.c ftview.c ftzoom.c ftdump.c ftstring.c ftstrpnm.c \
      ftsbit.c common.c blitter.c ftmetric.c ftstrtto.c arabic.c textdisp.c

GSRC = gmain.c display.c blitter.c
GFSSRC = $(GSRC) $(GFSDRIVER)
GPMSRC = $(GSRC) $(GPMDRIVER)

GFSOBJ = $(GFSSRC:.c=.o)
GPMOBJ = $(GPMSRC:.c=.o)

ALLSRC = $(SRC) $(GSRC) $(GFSDRIVER) $(GPMDRIVER)
ALLOBJ = $(ALLSRC:.c=.o)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.exe:
	$(CC) $(CFLAGS) -o $@ @$(mktmp $(&:t"\n")\n)


EXEFILES = ftview.exe ftviewfs.exe \
           ftlint.exe \
           fttimer.exe fttimefs.exe \
           ftdump.exe \
           ftstring.exe ftstrfs.exe \
           ftzoom.exe ftzoomfs.exe \
           ftstrpnm.exe \
           ftsbit.exe \
           ftmetric.exe \
           ftstrtto.exe ftstrtfs.exe

.PHONY: all debug freetype freetype_debug \
        clean distclean do_clean do_distclean depend


all: freetype $(EXEFILES)

debug: freetype_debug $(EXEFILES)


freetype:
[
	cd $(LIBDIR_OS2)
	$(FT_MAKE) -f $(FT_MAKEFILE) all
	cd $(MAKEDIR)
]

freetype_debug:
[
	cd $(LIBDIR_OS2)
	$(FT_MAKE) -f $(FT_MAKEFILE) debug
	cd $(MAKEDIR)
]

ftzoom.exe: $(GPMOBJ) ftzoom.o common.o $(LIBDIR)/libttf.a $(GPM_DEF)
ftzoomfs.exe: $(GFSOBJ) ftzoom.o common.o $(LIBDIR)/libttf.a
ftview.exe: $(GPMOBJ) ftview.o common.o $(LIBDIR)/libttf.a $(GPM_DEF)
ftviewfs.exe: $(GFSOBJ) ftview.o common.o $(LIBDIR)/libttf.a
ftlint.exe: ftlint.o common.o $(LIBDIR)/libttf.a
ftdump.exe: ftdump.o common.o $(LIBDIR)/libttf.a
ftstring.exe: $(GPMOBJ) ftstring.o common.o $(LIBDIR)/libttf.a $(GPM_DEF)
ftstrfs.exe: $(GFSOBJ) ftstring.o common.o $(LIBDIR)/libttf.a
ftstrpnm.exe: ftstrpnm.o common.o $(LIBDIR)/libttf.a
fttimer.exe: $(GPMOBJ) fttimer.o common.o $(LIBDIR)/libttf.a $(GPM_DEF)
fttimefs.exe: $(GFSOBJ) fttimer.o common.o $(LIBDIR)/libttf.a
ftsbit.exe: ftsbit.o common.o textdisp.o $(LIBDIR)/libttf.a
ftmetric.exe: ftmetric.o common.o textdisp.o $(LIBDIR)/libttf.a
ftstrtto.exe: $(GPMOBJ) ftstrtto.o common.o arabic.o \
              $(LIBDIR)/libttf.a $(GPM_DEF)
ftstrtfs.exe: $(GFSOBJ) ftstrtto.o common.o arabic.o $(LIBDIR)/libttf.a


clean: do_clean
[
	cd $(LIBDIR_OS2)
	$(FT_MAKE) -f $(FT_MAKEFILE) clean
	cd $(MAKEDIR)
]

distclean: do_clean do_distclean
[
	cd $(LIBDIR_OS2)
	$(FT_MAKE) -f $(FT_MAKEFILE) distclean
	cd $(MAKEDIR)
]

do_distclean:
	-+del dep.end $(EXEFILES) core

do_clean:
	-+del $(subst,/,\ $(ALLOBJ))

# depend: $(ALLSRC)
#[
#	cd $(LIBDIR_OS2)
#	$(FT_MAKE) -f $(FT_MAKEFILE) depend
#	cd $(MAKEDIR)
#	$(CC) -E -M $(INCDIRS) @$(mktmp $(<:t"\n")\n) > dep.end
#]
#
# ifeq (dep.end,$(wildcard dep.end))
#   include dep.end
# endif

# end of Makefile.dm
