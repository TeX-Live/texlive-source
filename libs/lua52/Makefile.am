## Proxy Makefile.am to build lua52 for TeX Live.
##
##   Copyright (C) 2013 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
ACLOCAL_AMFLAGS = -I ../../m4

## We want to re-distribute the whole original zlib source tree.
##
EXTRA_DIST = $(LUA52_TREE)

## Changes applied to the original source tree
##
EXTRA_DIST += $(LUA52_TREE)-PATCHES

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn -o -name .deps -o -name .dirstamp -o -name '*.$(OBJEXT)'`

SUBDIRS = . include

AM_CPPFLAGS = -I$(top_srcdir)/$(LUA52_TREE)/src $(LUA52_DEFINES)
AM_CFLAGS = $(WARNING_CFLAGS)

noinst_LIBRARIES = liblua52.a

nodist_liblua52_a_SOURCES = \
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

# Rebuild
rebuild_prereq =
rebuild_target = all
CLEANFILES =

include $(srcdir)/../../am/rebuild.am

