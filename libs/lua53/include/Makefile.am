## Proxy Makefile.am to install lua53 headers for TeX Live.
##
##   Copyright (C) 2013 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
LUA53_SRC = $(top_srcdir)/$(LUA53_TREE)/src

hdr_links = \
	$(LUA53_SRC)/lapi.h \
	$(LUA53_SRC)/lauxlib.h \
	$(LUA53_SRC)/lcode.h \
	$(LUA53_SRC)/lctype.h \
	$(LUA53_SRC)/ldebug.h \
	$(LUA53_SRC)/ldo.h \
	$(LUA53_SRC)/lfunc.h \
	$(LUA53_SRC)/lgc.h \
	$(LUA53_SRC)/llex.h \
	$(LUA53_SRC)/llimits.h \
	$(LUA53_SRC)/lmem.h \
	$(LUA53_SRC)/lobject.h \
	$(LUA53_SRC)/lopcodes.h \
	$(LUA53_SRC)/lparser.h \
	$(LUA53_SRC)/lstate.h \
	$(LUA53_SRC)/lstring.h \
	$(LUA53_SRC)/ltable.h \
	$(LUA53_SRC)/ltm.h \
	$(LUA53_SRC)/luaconf.h \
	$(LUA53_SRC)/lua.h \
	$(LUA53_SRC)/lua.hpp \
	$(LUA53_SRC)/lualib.h \
	$(LUA53_SRC)/lundump.h \
	$(LUA53_SRC)/lvm.h \
	$(LUA53_SRC)/lzio.h

include $(top_srcdir)/../../am/hdr_links.am

DISTCLEANFILES = lua.hpp

