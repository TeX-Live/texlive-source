## $Id$
## Makefile.am for the TeX Live subdirectory utils/utfpatgen/
##
## Copyright 2026 Karl Berry <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.

## We want to re-distribute the whole original vlna source tree.
EXTRA_DIST = $(UTFPATGEN_TREE)

## Patches applied to the original source tree
EXTRA_DIST += TLpatches

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am

AM_CFLAGS = $(WARNING_CFLAGS)

# for utfpatgen.h.
AM_CPPFLAGS = -I$(srcdir)/$(UTFPATGEN_TREE)

bin_PROGRAMS = utfpatgen
man1_MANS = $(UTFPATGEN_TREE)/utfpatgen.man

## Tests.
##
TESTS = tests/utfpatgen-vh.test
EXTRA_DIST += $(TESTS)
tests/utfpatgen.log: utfpatgen$(EXEEXT)

# Remake the .c from .w with make TL_CTANGLE=1, which we do by hand.
# Since this has to be done in the build dir, yet we want utfpatgen.c in
# the source dir, copy it afterwards.
 ifdef TL_CTANGLE
utfpatgen.c: $(UTFPATGEN_TREE)/utfpatgen.w
	$(CTANGLE) $<
	cp utfpatgen.c $(srcdir)/$(UTFPATGEN_TREE)/../utfpatgen.c
# the leading space here stops Automake from reading it:
 endif #TL_CTANGLE
