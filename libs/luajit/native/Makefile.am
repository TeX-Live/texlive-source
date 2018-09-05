## Makefile.am for the TeX Live subdirectory libs/luajit/native/
##
## Copyright (C) 2014 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
AM_CPPFLAGS = -I$(srcdir)/$(LUAJIT_TREE)/src
AM_CFLAGS = -Wall

EXTRA_PROGRAMS = buildvm minilua

all-local: buildvm-stamp

buildvm-stamp: buildvm$(EXEEXT)
	$(AM_V_at)echo timestamp >$@

buildvm_CPPFLAGS = $(AM_CPPFLAGS) $(LUAJIT_DEFINES) `cat ../native_flags`
nodist_buildvm_SOURCES = \
	@LUAJIT_TREE@/src/host/buildvm.c \
	@LUAJIT_TREE@/src/host/buildvm.h \
	@LUAJIT_TREE@/src/host/buildvm_asm.c \
	@LUAJIT_TREE@/src/host/buildvm_fold.c \
	@LUAJIT_TREE@/src/host/buildvm_lib.c \
	@LUAJIT_TREE@/src/host/buildvm_peobj.c
$(buildvm_OBJECTS): buildvm_arch.h

buildvm_arch.h: minilua$(EXEEXT) $(LUAJIT_TREE)/dynasm/dynasm.lua
	$(AM_V_GEN)./minilua $(srcdir)/$(LUAJIT_TREE)/dynasm/dynasm.lua \
	  `cat ../dynasm_flags` \
	  -o $@ $(srcdir)/$(LUAJIT_TREE)/src/vm_$(DASM_ARCH).dasc

minilua_CPPFLAGS = $(AM_CPPFLAGS) $(LUAJIT_DEFINES) `cat ../native_flags`
nodist_minilua_SOURCES = \
	@LUAJIT_TREE@/src/host/minilua.c
minilua_LDADD = $(MATH_LIB)

CLEANFILES = buildvm-stamp buildvm_arch.h

