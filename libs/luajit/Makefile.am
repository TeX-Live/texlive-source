## Proxy Makefile.am to build LuaJIT for TeX Live.
##
## Copyright (C) 2014 Luigi Scarso <luigi.scarso@gmail.com>
## Copyright (C) 2014, 2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
## We want to re-distribute the whole original LuaJIT source tree.
##
EXTRA_DIST = $(LUAJIT_TREE)

## Changes applied to the original source tree
##
EXTRA_DIST += $(LUAJIT_TREE)-PATCHES

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB) $(NEVER_NAMES_LT)

AM_CPPFLAGS = -I$(srcdir)/$(LUAJIT_TREE)/src $(LUAJIT_DEFINES) -U_FORTIFY_SOURCE
AM_CFLAGS = $(LUAJIT_CFLAGS) -Wall
AM_CCASFLAGS = $(LUAJIT_CFLAGS)

SUBDIRS = . native include

lib_LTLIBRARIES = libtexluajit.la

libtexluajit_la_LDFLAGS = -bindir @bindir@ -no-undefined -version-info $(LUAJIT_LT_VERSINFO) $(LIBLUAJIT_LDEXTRA)

dist_libtexluajit_la_SOURCES = \
	lbitlib.c

nodist_libtexluajit_la_SOURCES = \
	$(ljlib_sources) \
	@LUAJIT_TREE@/src/lib_aux.c \
	@LUAJIT_TREE@/src/lib_init.c \
	@LUAJIT_TREE@/src/lj_alloc.c \
	@LUAJIT_TREE@/src/lj_api.c \
	@LUAJIT_TREE@/src/lj_asm.c \
	@LUAJIT_TREE@/src/lj_bc.c \
	@LUAJIT_TREE@/src/lj_bcread.c \
	@LUAJIT_TREE@/src/lj_bcwrite.c \
	@LUAJIT_TREE@/src/lj_buf.c \
	@LUAJIT_TREE@/src/lj_carith.c \
	@LUAJIT_TREE@/src/lj_ccall.c \
	@LUAJIT_TREE@/src/lj_ccallback.c \
	@LUAJIT_TREE@/src/lj_cconv.c \
	@LUAJIT_TREE@/src/lj_cdata.c \
	@LUAJIT_TREE@/src/lj_char.c \
	@LUAJIT_TREE@/src/lj_clib.c \
	@LUAJIT_TREE@/src/lj_cparse.c \
	@LUAJIT_TREE@/src/lj_crecord.c \
	@LUAJIT_TREE@/src/lj_ctype.c \
	@LUAJIT_TREE@/src/lj_debug.c \
	@LUAJIT_TREE@/src/lj_dispatch.c \
	@LUAJIT_TREE@/src/lj_err.c \
	@LUAJIT_TREE@/src/lj_ffrecord.c \
	@LUAJIT_TREE@/src/lj_func.c \
	@LUAJIT_TREE@/src/lj_gc.c \
	@LUAJIT_TREE@/src/lj_gdbjit.c \
	@LUAJIT_TREE@/src/lj_ir.c \
	@LUAJIT_TREE@/src/lj_lex.c \
	@LUAJIT_TREE@/src/lj_lib.c \
	@LUAJIT_TREE@/src/lj_load.c \
	@LUAJIT_TREE@/src/lj_mcode.c \
	@LUAJIT_TREE@/src/lj_meta.c \
	@LUAJIT_TREE@/src/lj_obj.c \
	@LUAJIT_TREE@/src/lj_opt_dce.c \
	@LUAJIT_TREE@/src/lj_opt_fold.c \
	@LUAJIT_TREE@/src/lj_opt_loop.c \
	@LUAJIT_TREE@/src/lj_opt_mem.c \
	@LUAJIT_TREE@/src/lj_opt_narrow.c \
	@LUAJIT_TREE@/src/lj_opt_sink.c \
	@LUAJIT_TREE@/src/lj_opt_split.c \
	@LUAJIT_TREE@/src/lj_parse.c \
	@LUAJIT_TREE@/src/lj_profile.c \
	@LUAJIT_TREE@/src/lj_record.c \
	@LUAJIT_TREE@/src/lj_snap.c \
	@LUAJIT_TREE@/src/lj_state.c \
	@LUAJIT_TREE@/src/lj_str.c \
	@LUAJIT_TREE@/src/lj_strfmt.c \
	@LUAJIT_TREE@/src/lj_strscan.c \
	@LUAJIT_TREE@/src/lj_tab.c \
	@LUAJIT_TREE@/src/lj_trace.c \
	@LUAJIT_TREE@/src/lj_udata.c \
	@LUAJIT_TREE@/src/lj_vmevent.c \
	@LUAJIT_TREE@/src/lj_vmmath.c

if PEOBJ
libtexluajit_la_LIBADD = lj_vm_obj.lo
else !PEOBJ
nodist_libtexluajit_la_SOURCES += lj_vm_asm.S
endif !PEOBJ

ljlib_sources = \
	lib_base.c \
	lib_math.c \
	lib_bit.c \
	lib_string.c \
	lib_table.c \
	lib_io.c \
	lib_os.c \
	lib_package.c \
	lib_debug.c \
	lib_jit.c \
	lib_ffi.c

$(libtexluajit_la_OBJECTS): $(HDRGEN)

# When cross-compiling, EXEEXT for the build and host systems may differ.
# Thus we depend on the auxiliary file native/buildvm-stamp.
native/buildvm-stamp: $(ljlib_sources) lj_opt_fold.c
	cd native && $(MAKE) $(AM_MAKEFLAGS) buildvm-stamp

HDRGEN = lj_bcdef.h lj_ffdef.h lj_libdef.h lj_recdef.h lj_folddef.h

lj_bcdef.h: native/buildvm-stamp
	$(AM_V_GEN)native/buildvm -m bcdef -o $@ $(ljlib_sources)

lj_ffdef.h: native/buildvm-stamp
	$(AM_V_GEN)native/buildvm -m ffdef -o $@ $(ljlib_sources)

lj_libdef.h: native/buildvm-stamp
	$(AM_V_GEN)native/buildvm -m libdef -o $@ $(ljlib_sources)

lj_recdef.h: native/buildvm-stamp
	$(AM_V_GEN)native/buildvm -m recdef -o $@ $(ljlib_sources)

lj_folddef.h: native/buildvm-stamp
	$(AM_V_GEN)native/buildvm -m folddef -o $@ lj_opt_fold.c

lj_vm_obj.o lj_vm_asm.S: native/buildvm-stamp
	$(AM_V_GEN)native/buildvm -m $(LJVM_MODE) -o $@

lj_vm_obj.lo: lj_vm_obj.o lib_base.lo
	$(AM_V_GEN)sed 's/lib_base/lj_vm_obj/' lib_base.lo >$@ \
	  && . ./$@ && test x"$$pic_object" == xnone \
	  || cp lj_vm_obj.o .libs/lj_vm_obj.o

$(ljlib_sources) lj_opt_fold.c: config.force
	@test -f $@ || { rm -f $@; \
	  if $(AM_V_P); then echo "$(LN_S) $(srcdir)/$(LUAJIT_TREE)/src/$@ $@"; \
	    else echo "  LINK     $@"; fi; \
	  $(LN_S) $(srcdir)/$(LUAJIT_TREE)/src/$@ $@; } || exit 1

CLEANFILES = $(HDRGEN) $(ljlib_sources) lj_opt_fold.c lj_vm_asm.S

CONFIG_CLEAN_FILES = dynasm_flags native_flags system_flags

luajitincludedir = ${includedir}/texluajit

luajitinclude_HEADERS = \
	$(LUAJIT_TREE)/src/lauxlib.h \
	$(LUAJIT_TREE)/src/luaconf.h \
	$(LUAJIT_TREE)/src/luajit.h \
	$(LUAJIT_TREE)/src/lua.h \
	$(LUAJIT_TREE)/src/lua.hpp \
	$(LUAJIT_TREE)/src/lualib.h

pkgconfigdir = ${libdir}/pkgconfig

pkgconfig_DATA = texluajit.pc

if build
check_PROGRAMS = jittest luajittry
dist_check_SCRIPTS = luajit.test luajiterr.test
TESTS = luajit.test luajiterr.test
endif build
luajit.log: jittest$(EXEEXT)
luajiterr.log: luajittry$(EXEEXT)

jittest_SOURCES = jittest.c

jittest_CPPFLAGS = -Iinclude $(LUAJIT_DEFINES)

nodist_luajittry_SOURCES = @LUAJIT_TREE@/src/luajit.c

luajittry_CPPFLAGS = -Iinclude $(LUAJIT_DEFINES)
luajittry_LDFLAGS = $(LUAJIT_LDEXTRA)

LDADD = libtexluajit.la

EXTRA_DIST += jiterr.exp
CLEANFILES += jiterr.out jiterr.tmp

# Reconfig
reconfig_prereq =
DISTCLEANFILES =

include $(srcdir)/../../am/reconfig.am

# Rebuild
rebuild_prereq =
rebuild_target = all

include $(srcdir)/../../am/rebuild.am

