## Proxy Makefile.am to build Lua52 for TeX Live.
##
##   Copyright (C) 2013-2015 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
## We want to re-distribute the whole Lua52 source tree.
##
EXTRA_DIST = $(LUA52_TREE)

## Changes applied to the original source tree
##
EXTRA_DIST += $(LUA52_TREE)-PATCHES

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB) $(NEVER_NAMES_LT)

SUBDIRS = . include

AM_CPPFLAGS = -I$(top_srcdir)/$(LUA52_TREE)/src $(LUA52_DEFINES)
AM_CFLAGS = $(WARNING_CFLAGS)

lib_LTLIBRARIES = libtexlua52.la

libtexlua52_la_LDFLAGS = -bindir @bindir@ -no-undefined -version-info $(LUA52_LT_VERSINFO)

nodist_libtexlua52_la_SOURCES = \
	@LUA52_TREE@/src/lapi.c \
	@LUA52_TREE@/src/lauxlib.c \
	@LUA52_TREE@/src/lbaselib.c \
	@LUA52_TREE@/src/lbitlib.c \
	@LUA52_TREE@/src/lcode.c \
	@LUA52_TREE@/src/lcorolib.c \
	@LUA52_TREE@/src/lctype.c \
	@LUA52_TREE@/src/ldblib.c \
	@LUA52_TREE@/src/ldebug.c \
	@LUA52_TREE@/src/ldo.c \
	@LUA52_TREE@/src/ldump.c \
	@LUA52_TREE@/src/lfunc.c \
	@LUA52_TREE@/src/lgc.c \
	@LUA52_TREE@/src/linit.c \
	@LUA52_TREE@/src/liolib.c \
	@LUA52_TREE@/src/llex.c \
	@LUA52_TREE@/src/lmathlib.c \
	@LUA52_TREE@/src/lmem.c \
	@LUA52_TREE@/src/loadlib.c \
	@LUA52_TREE@/src/lobject.c \
	@LUA52_TREE@/src/lopcodes.c \
	@LUA52_TREE@/src/loslib.c \
	@LUA52_TREE@/src/lparser.c \
	@LUA52_TREE@/src/lstate.c \
	@LUA52_TREE@/src/lstring.c \
	@LUA52_TREE@/src/lstrlib.c \
	@LUA52_TREE@/src/ltable.c \
	@LUA52_TREE@/src/ltablib.c \
	@LUA52_TREE@/src/ltm.c \
	@LUA52_TREE@/src/luaconf.h \
	@LUA52_TREE@/src/lundump.c \
	@LUA52_TREE@/src/lvm.c \
	@LUA52_TREE@/src/lzio.c

lua52includedir = ${includedir}/texlua52

lua52include_HEADERS = \
	$(LUA52_TREE)/src/lauxlib.h \
	$(LUA52_TREE)/src/luaconf.h \
	$(LUA52_TREE)/src/lua.h \
	$(LUA52_TREE)/src/lua.hpp \
	$(LUA52_TREE)/src/lualib.h

pkgconfigdir = ${libdir}/pkgconfig

pkgconfig_DATA = texlua52.pc

if build
check_PROGRAMS = luatest luatry
dist_check_SCRIPTS = lua52.test luaerror.test
TESTS = lua52.test luaerror.test
endif build

luatest_SOURCES = luatest.c

luatest_CPPFLAGS = -Iinclude

nodist_luatry_SOURCES = @LUA52_TREE@/src/lua.c

luatry_CPPFLAGS = -Iinclude

LDADD = libtexlua52.la

EXTRA_DIST += error.exp
CLEANFILES = error.out error.tmp

# Rebuild
rebuild_prereq =
rebuild_target = all

include $(srcdir)/../../am/rebuild.am

