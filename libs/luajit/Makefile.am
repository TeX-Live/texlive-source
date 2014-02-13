## Makefile.am for the TeX Live subdirectory libs/luajit/native/
##
## Copyright (C) 2014 Luigi Scarso <luigi.scarso@gmail.com>
## Copyright (C) 2014 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
## We want to re-distribute the whole original luajit source tree.
##
EXTRA_DIST = $(LUAJIT_TREE)

## Changes applied to the original source tree
##
EXTRA_DIST += $(LUAJIT_TREE)-PATCHES

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`

AM_CFLAGS = # $(WARNING_CFLAGS)

SUBDIRS = native . include

## host_flags = BUILDMODE=static CROSS=$(CROSS)
## if WIN32
## host_flags += TARGET_SYS=Windows
## endif WIN32

## noinst_DATA = luajit-build/src/libluajit.a

## luajit-build/src/Makefile:
## 	rm -rf luajit-build
## 	$(MKDIR_P) luajit-build
## 	cd luajit-build && cp -r $(abs_srcdir)/$(LUAJIT_TREE)/src src \
## 	&& cp -r $(abs_srcdir)/$(LUAJIT_TREE)/dynasm dynasm \
## 	&& chmod -R u+w src dynasm

## native/build-flags: luajit-build/src/Makefile
## 	cd native && $(MAKE) $(AM_MAKEFLAGS) build-flags

## luajit-build/src/libluajit.a: native/build-flags
## 	cat native/build-flags
## 	cd luajit-build/src && \
## 	  $(MAKE) $(AM_MAKEFLAGS) `if $(AM_V_P); then echo 'E=@: Q='; fi` \
## 	  $(host_flags) $$build_flags

noinst_LIBRARIES = libluajit.a

nodist_libluajit_a_SOURCES = $(ljlib_sources) $(ljcore_sources)

if PEOBJ
libluajit_a_LIBADD = lj_vm_obj.o
else !PEOBJ
nodist_libluajit_a_SOURCES += lj_vm_asm.s
endif !PEOBJ

$(libluajit_a_OBJECTS): $(HDRGEN)

# When cross-compiling, EXEEXT for the build and host systems may differ.
# Thus we depend on the auxiliary file native/buildvm-stamp.
native/buildvm-stamp:
	cd native && $(MAKE) $(AM_MAKEFLAGS) buildvm-stamp

HDRGEN = lj_bcdef.h lj_ffdef.h lj_libdef.h lj_recdef.h lj_folddef.h

lj_bcdef.h: native/buildvm-stamp $(ljlib_sources)
	$(AM_V_GEN)native/buildvm -m bcdef -o $@ $(ljlib_sources)

lj_ffdef.h: native/buildvm-stamp $(ljlib_sources)
	$(AM_V_GEN)native/buildvm -m ffdef -o $@ $(ljlib_sources)

lj_libdef.h: native/buildvm-stamp $(ljlib_sources)
	$(AM_V_GEN)native/buildvm -m libdef -o $@ $(ljlib_sources)

lj_recdef.h: native/buildvm-stamp $(ljlib_sources)
	$(AM_V_GEN)native/buildvm -m recdef -o $@ $(ljlib_sources)

lj_folddef.h: native/buildvm-stamp lj_opt_fold.c
	$(AM_V_GEN)native/buildvm -m folddef -o $@ lj_opt_fold.c

lj_vm_obj.o lj_vm_asm.s: native/buildvm-stamp
	$(AM_V_GEN)native/buildvm -m $(LJVM_MODE) -o $@

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

ljcore_sources = \
	lib_aux.c \
	lib_init.c \
	lj_alloc.c \
	lj_api.c \
	lj_asm.c \
	lj_bc.c \
	lj_bcread.c \
	lj_bcwrite.c \
	lj_carith.c \
	lj_ccall.c \
	lj_ccallback.c \
	lj_cconv.c \
	lj_cdata.c \
	lj_char.c \
	lj_clib.c \
	lj_cparse.c \
	lj_crecord.c \
	lj_ctype.c \
	lj_debug.c \
	lj_dispatch.c \
	lj_err.c \
	lj_ffrecord.c \
	lj_func.c \
	lj_gc.c \
	lj_gdbjit.c \
	lj_ir.c \
	lj_lex.c \
	lj_lib.c \
	lj_load.c \
	lj_mcode.c \
	lj_meta.c \
	lj_obj.c \
	lj_opt_dce.c \
	lj_opt_fold.c \
	lj_opt_loop.c \
	lj_opt_mem.c \
	lj_opt_narrow.c \
	lj_opt_sink.c \
	lj_opt_split.c \
	lj_parse.c \
	lj_record.c \
	lj_snap.c \
	lj_state.c \
	lj_str.c \
	lj_strscan.c \
	lj_tab.c \
	lj_trace.c \
	lj_udata.c \
	lj_vmevent.c \
	lj_vmmath.c

$(ljlib_sources) $(ljcore_sources):
	@test -f $@ || { rm -f $@; \
	  if $(AM_V_P); then echo "$(LN_S) $(srcdir)/$(LUAJIT_TREE)/src/$@ $@"; \
	    else echo "  INST     $@"; fi; \
	  $(LN_S) $(srcdir)/$(LUAJIT_TREE)/src/$@ $@; } || exit 1

all-local: $(HDRGEN)

clean-local:
	rm -f $(nodist_libluajit_a_SOURCES)
## 	rm -rf luajit-build

if build
check_PROGRAMS = jittest
dist_check_SCRIPTS = luajit.test
TESTS = luajit.test
endif build

jittest_SOURCES = jittest.c

jittest_CPPFLAGS = -Iinclude

LDADD = libluajit.a

# Rebuild
rebuild_prereq =
rebuild_target = all
CLEANFILES = $(HDRGEN)

CONFIG_CLEAN_FILES = dynasm_flags native_flags

include $(srcdir)/../../am/rebuild.am

