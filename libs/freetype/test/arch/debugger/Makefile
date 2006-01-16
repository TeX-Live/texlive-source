# This file is part of the FreeType project.
#
# It builds the debugger for emx-gcc under OS/2 resp. under Unix.
#
# You will need GNU make.
#
# Use this file while in the `test' directory with the following statements:
#
#   make -f arch/debugger/Makefile depend
#   make -f arch/debugger/Makefile
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

ARCH = arch/debugger
FT_MAKEFILE = $(ARCH)/Makefile

CC = gcc

LIBDIR = ../lib
LIBTTF = $(LIBDIR)/$(ARCH)/libttf.a

INCDIRS = -I$(LIBDIR) -I$(LIBDIR)/extend -I$(LIBDIR)/$(ARCH) -I.

# Note: The debugger uses non-ANSI functions to read the keyboard
#       on OS/2 -- do not set the `-ansi flag here.
#
CFLAGS = -Wall -O0 -g $(INCDIRS)


SRC = fdebug.c common.c

ALLSRC = $(SRC)
ALLOBJ = $(ALLSRC:.c=.o)

# on OS/2, do not use the curses library
#
ifdef OS2_SHELL
EXE        := fdebug.exe
OS         := OS2
EFENCE     :=
EXTRAFLAGS :=
RM         := del
else
EXE        := fdebug
OS         := UNIX
EFENCE     := -lefence
RM         := rm -f
#
# POSIX TERMIOS: Do not define if you use OLD U*ix like 4.2BSD.
#
EXTRAFLAGS := HAVE_POSIX_TERMIOS
endif

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $< -D$(OS) -D$(EXTRAFLAGS)

%.exe:
	$(CC) $(CFLAGS) -o $@ $^


	
EXEFILES = $(EXE)

.PHONY: all debug freetype freetype_debug \
        clean distclean do_clean depend


all: freetype $(EXEFILES)

debug: freetype_debug $(EXEFILES)

$(EXE): fdebug.o common.o $(LIBTTF)
$(EXE):
	$(CC) $(CFLAGS) -o $@ $^ $(EFENCE)

freetype:
	$(MAKE) -C $(LIBDIR) -f $(FT_MAKEFILE) all

freetype_debug:
	$(MAKE) -C $(LIBDIR) -f $(FT_MAKEFILE) debug


clean: do_clean
	$(MAKE) -C $(LIBDIR) -f $(FT_MAKEFILE) clean

distclean: do_clean
	$(MAKE) -C $(LIBDIR) -f $(FT_MAKEFILE) distclean
	-$(RM) dep.end $(EXEFILES) core

do_clean:
	-$(RM) $(subst /,\,$(ALLOBJ))


depend: $(ALLSRC)
	$(MAKE) -C $(LIBDIR) -f $(FT_MAKEFILE) depend
	$(CC) -E -MM $(INCDIRS) $^ > dep.end

ifeq (dep.end,$(wildcard dep.end))
  include dep.end
endif

# end of Makefile.emx
