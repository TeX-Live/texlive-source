## Makefile.am for the TeX Live subdirectory libs/luajit/native/
##
## Copyright (C) 2014 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
noinst_DATA = build-flags

build_flags = HOST_CC='$(CC)'
build_flags += HOST_CFLAGS='$(CFLAGS)'
build_flags += HOST_LDFLAGS='$(LDFLAGS)'
build_flags += HOST_LIBS='$(LIBS)'

build-flags: Makefile
	$(AM_V_GEN)echo "$(build_flags)" >$@

