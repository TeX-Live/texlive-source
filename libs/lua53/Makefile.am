## $Id: Makefile.am 43567 2017-03-21 17:43:10Z karl $
## Proxy Makefile.am to build Lua53 for TeX Live.
##
##   Copyright 2017 Karl Berry <tex-live@tug.org>
##   Copyright 2013-2015 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
## We want to re-distribute the whole Lua53 source tree.
##
EXTRA_DIST = $(LUA53_TREE)

## Changes applied to the original source tree
##
EXTRA_DIST += TLpatches

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB) $(NEVER_NAMES_LT)

SUBDIRS = . include

AM_CPPFLAGS = -I$(top_srcdir)/$(LUA53_TREE)/src $(LUA53_DEFINES)
AM_CFLAGS = $(WARNING_CFLAGS)  $(LUA53_CFLAGS)

lib_LTLIBRARIES = libtexlua53.la

libtexlua53_la_LDFLAGS = -bindir @bindir@ -no-undefined -version-info $(LUA53_LT_VERSINFO)

nodist_libtexlua53_la_SOURCES = \
	@LUA53_TREE@/src/lapi.c \
	@LUA53_TREE@/src/lauxlib.c \
	@LUA53_TREE@/src/lbaselib.c \
	@LUA53_TREE@/src/lbitlib.c \
	@LUA53_TREE@/src/lcode.c \
	@LUA53_TREE@/src/lcorolib.c \
	@LUA53_TREE@/src/lctype.c \
	@LUA53_TREE@/src/ldblib.c \
	@LUA53_TREE@/src/ldebug.c \
	@LUA53_TREE@/src/ldo.c \
	@LUA53_TREE@/src/ldump.c \
	@LUA53_TREE@/src/lfunc.c \
	@LUA53_TREE@/src/lgc.c \
	@LUA53_TREE@/src/linit.c \
	@LUA53_TREE@/src/liolib.c \
	@LUA53_TREE@/src/llex.c \
	@LUA53_TREE@/src/lmathlib.c \
	@LUA53_TREE@/src/lmem.c \
	@LUA53_TREE@/src/loadlib.c \
	@LUA53_TREE@/src/lobject.c \
	@LUA53_TREE@/src/lopcodes.c \
	@LUA53_TREE@/src/loslib.c \
	@LUA53_TREE@/src/lparser.c \
	@LUA53_TREE@/src/lstate.c \
	@LUA53_TREE@/src/lstring.c \
	@LUA53_TREE@/src/lstrlib.c \
	@LUA53_TREE@/src/ltable.c \
	@LUA53_TREE@/src/ltablib.c \
	@LUA53_TREE@/src/ltm.c \
	@LUA53_TREE@/src/lua.c \
	@LUA53_TREE@/src/lundump.c \
	@LUA53_TREE@/src/lutf8lib.c \
	@LUA53_TREE@/src/lvm.c \
	@LUA53_TREE@/src/lzio.c 


lua53includedir = ${includedir}/texlua53

lua53include_HEADERS = \
	$(LUA53_TREE)/src/lauxlib.h \
	$(LUA53_TREE)/src/luaconf.h \
	$(LUA53_TREE)/src/lua.h \
	$(LUA53_TREE)/src/lua.hpp \
	$(LUA53_TREE)/src/lualib.h

pkgconfigdir = ${libdir}/pkgconfig

pkgconfig_DATA = texlua53.pc

if build
check_PROGRAMS = luatest luatry
dist_check_SCRIPTS = lua53.test luaerror.test
TESTS = lua53.test luaerror.test
endif build
lua53.log: luatest$(EXEEXT)
luaerror.log: luatry$(EXEEXT)

luatest_SOURCES = luatest.c

luatest_CPPFLAGS = -Iinclude

nodist_luatry_SOURCES = @LUA53_TREE@/src/lua.c

luatry_CPPFLAGS = -Iinclude

LDADD = libtexlua53.la

EXTRA_DIST += error.exp
CLEANFILES = error.out error.tmp

# Rebuild
rebuild_prereq =
rebuild_target = all

include $(srcdir)/../../am/rebuild.am

