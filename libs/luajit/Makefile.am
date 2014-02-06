## Wrapper Makefile.am to build libluajit.
##
##   Copyright (C) 2014 Luigi Scarso <luigi.scarso@gmail.com>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
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

SUBDIRS = native

host_flags = BUILDMODE=static CROSS=$(CROSS)
if WIN32
host_flags += TARGET_SYS=Windows
endif WIN32

noinst_DATA = luajit-build/src/libluajit.a

luajit-build/src/Makefile:
	rm -rf luajit-build
	$(MKDIR_P) luajit-build
	cd luajit-build && cp -r $(abs_srcdir)/$(LUAJIT_TREE)/src src \
	&& cp -r $(abs_srcdir)/$(LUAJIT_TREE)/dynasm dynasm \
	&& chmod -R u+w src dynasm

native/build-flags: luajit-build/src/Makefile
	cd native && $(MAKE) $(AM_MAKEFLAGS) build-flags

luajit-build/src/libluajit.a: native/build-flags
	build_flags=`cat native/build-flags` && \
	cat native/build-flags
	cd luajit-build/src && \
	  $(MAKE) $(AM_MAKEFLAGS) `if $(AM_V_P); then echo 'E=@: Q='; fi` \
	  $(host_flags) $$build_flags

clean-local:
	rm -rf luajit-build

# Rebuild
rebuild_prereq =
rebuild_target = all
CLEANFILES =

include $(srcdir)/../../am/rebuild.am

