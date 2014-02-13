## Proxy Makefile.am to install luajit headers for TeX Live.
##
##   Copyright (C) 2014 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
LUAJIT_SRC = $(top_srcdir)/$(LUAJIT_TREE)/src

hdr_links = \
	$(LUAJIT_SRC)/lauxlib.h \
	$(LUAJIT_SRC)/lj_arch.h \
	$(LUAJIT_SRC)/lj_def.h \
	$(LUAJIT_SRC)/lj_obj.h \
	$(LUAJIT_SRC)/lj_state.h \
	$(LUAJIT_SRC)/lua.h \
	$(LUAJIT_SRC)/luaconf.h \
	$(LUAJIT_SRC)/luajit.h \
	$(LUAJIT_SRC)/lualib.h

include $(top_srcdir)/../../am/hdr_links.am

