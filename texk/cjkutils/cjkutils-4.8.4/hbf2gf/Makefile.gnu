# This file is part of the CJK package Version 4.8.4 (18-Apr-2015)

# Copyright (C) 1994-2015  Werner Lemberg <wl@gnu.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program in doc/COPYING; if not, write to the Free
# Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
# MA 02110-1301 USA

# GNU Makefile for hbf2gf

.PHONY: default all debug documentation clean \
        do_all

.CAUTIOUS: hbf2gf.c

default:
	@echo " say                                     "
	@echo ".                                        "
	@echo "     make TARGET OS=SYSTEM               "
	@echo ".                                        "
	@echo " SYSTEM can be one of the following:     "
	@echo ".                                        "
	@echo "     os2 bound (emx)                     "
	@echo "     dos (djgpp)                         "
	@echo ".                                        "
	@echo " TARGET can be one of the following:     "
	@echo ".                                        "
	@echo "     all debug                           "
	@echo "     documentation (needs cweave package)"
	@echo "     clean                               "

ifdef OS

  # default values (bound executables built with emx)
  FS = msdos
  CC = gcc -Wall -O -DHAVE_EMTEXDIR
  LIB =
  O = .o
  EXE = .exe
  RM = del

  ifeq ($(OS),os2)
    FS = msdos
    CC = gcc -Wall -Zomf -Zmtd -O -DHAVE_EMTEXDIR
    LIB = hbf2gf.def
    O = .obj
    EXE = .exe
    RM = del
  endif

  ifeq ($(OS),dos)
    FS = msdos
    CC = gcc -Wall -O -DHAVE_EMTEXDIR
    LIB =
    O = .o
    EXE = .exe
    RM = del
  endif


  %$O: %.c
	$(CC) $(CFLAGS) -c -D$(FS) -o $@ $<

  %.c: %.w %.ch
	$(CTANGLE) $^ $@
  %.c: %.w
	$(CTANGLE) $<

  %.tex: %.w %.ch
	$(CWEAVE) +ai $^ $@
  %.tex: %.w
	$(CWEAVE) +ai $<

  %.dvi: %.tex
	$(TEX) $*


  all:
	$(MAKE) -f Makefile.gnu do_all CFLAGS=-s

  debug:
	$(MAKE) -f Makefile.gnu do_all CFLAGS=-g

  # this builds the .dvi-file
  documentation: hbf2gf.dvi

  # remove the unnecessary files;
  clean:
	-$(RM) *.scn
	-$(RM) *.toc
	-$(RM) *.idx
	-$(RM) *.log
	-$(RM) *.o


  do_all: hbf2gf$(EXE)

  hbf2gf$(EXE): hbf2gf$O hbf$O emdir$O emtexdir$O
    ifeq ($(OS),dos)
	$(CC) $(CFLAGS) -o $(basename $@) $^
	strip $(basename $@)
	coff2exe $(basename $@)
	-del $(basename $@)
    else
	$(CC) $(CFLAGS) -o $@ $^ $(LIB)
    endif

  hbf$O: hbf.c hbf.h
  emdir$O: emdir.c emdir.h
  emtexdir$O: emtexdir.c emtexdir.h emdir.h

else # ifdef OS

  all debug documentation clean: default

endif
