## Makefile.am for the TeX Live subdirectory libs/luajit/native/
##
## Copyright (C) 2014 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
AM_CPPFLAGS = -I$(srcdir)/$(LUAJIT_TREE)/src
AM_CFLAGS = # $(WARNING_CFLAGS)

noinst_DATA = build-flags

build_flags = HOST_CC='$(CC)'
build_flags += HOST_CFLAGS='$(CFLAGS)'
build_flags += HOST_LDFLAGS='$(LDFLAGS)'
build_flags += HOST_LIBS='$(LIBS)'

build-flags: Makefile
	$(AM_V_GEN)echo "$(build_flags)" >$@

EXTRA_PROGRAMS = buildvm minilua

all-local: buildvm-stamp

buildvm-stamp: buildvm$(EXEEXT)
	$(AM_V_at)echo timestamp >$@

buildvm_CPPFLAGS = $(AM_CPPFLAGS) -DLUAJIT_ENABLE_LUA52COMPAT `cat ../native_flags`
nodist_buildvm_SOURCES = \
	buildvm.c \
	buildvm.h \
	buildvm_asm.c \
	buildvm_fold.c \
	buildvm_lib.c \
	buildvm_peobj.c
$(buildvm_OBJECTS): $(nodist_buildvm_SOURCES) buildvm_arch.h

buildvm_arch.h: minilua$(EXEEXT)
	$(AM_V_GEN)./minilua $(srcdir)/$(LUAJIT_TREE)/dynasm/dynasm.lua \
	  `cat ../dynasm_flags` \
	  -o $@ $(srcdir)/$(LUAJIT_TREE)/src/vm_$(DASM_ARCH).dasc

nodist_minilua_SOURCES = minilua.c
$(minilua_OBJECTS): $(nodist_minilua_SOURCES)
minilua_LDFLAGS = $(MATH_LIB)

$(nodist_buildvm_SOURCES) $(nodist_minilua_SOURCES):
	@test -f $@ || { rm -f $@; \
	  if $(AM_V_P); then echo "$(LN_S) $(srcdir)/$(LUAJIT_TREE)/src/host/$@ $@"; \
	    else echo "  INST     $@"; fi; \
	  $(LN_S) $(srcdir)/$(LUAJIT_TREE)/src/host/$@ $@; } || exit 1

clean-local:
	rm -f $(nodist_buildvm_SOURCES) $(nodist_minilua_SOURCES)

CLEANFILES = buildvm-stamp buildvm_arch.h

