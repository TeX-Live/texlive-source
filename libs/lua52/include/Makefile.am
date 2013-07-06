## Proxy Makefile.am to install lua52 headers for TeX Live.
##
##   Copyright (C) 2013 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
LUA52_SRC = $(top_srcdir)/$(LUA52_TREE)/src

hdr_links = \
	$(LUA52_SRC)/lapi.h \
	$(LUA52_SRC)/lauxlib.h \
	$(LUA52_SRC)/lcode.h \
	$(LUA52_SRC)/lctype.h \
	$(LUA52_SRC)/ldebug.h \
	$(LUA52_SRC)/ldo.h \
	$(LUA52_SRC)/lfunc.h \
	$(LUA52_SRC)/lgc.h \
	$(LUA52_SRC)/llex.h \
	$(LUA52_SRC)/llimits.h \
	$(LUA52_SRC)/lmem.h \
	$(LUA52_SRC)/lobject.h \
	$(LUA52_SRC)/lopcodes.h \
	$(LUA52_SRC)/lparser.h \
	$(LUA52_SRC)/lstate.h \
	$(LUA52_SRC)/lstring.h \
	$(LUA52_SRC)/ltable.h \
	$(LUA52_SRC)/ltm.h \
	$(LUA52_SRC)/luaconf.h \
	$(LUA52_SRC)/lua.h \
	$(LUA52_SRC)/lua.hpp \
	$(LUA52_SRC)/lualib.h \
	$(LUA52_SRC)/lundump.h \
	$(LUA52_SRC)/lvm.h \
	$(LUA52_SRC)/lzio.h

include $(top_srcdir)/../../am/hdr_links.am

DISTCLEANFILES = lua.hpp

