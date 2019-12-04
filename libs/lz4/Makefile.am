## $Id: Makefile.am 43567 2017-03-21 17:43:10Z karl $
## Proxy Makefile.am to build lz4 for TeX Live.
##
##   Copyright 2019 Hironori Kitagawa
##   Copyright 2017 Karl Berry <tex-live@tug.org>
##   Copyright 2013-2015 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
## We want to re-distribute the whole Lz4 source tree.
##
EXTRA_DIST = $(LZ4_TREE)

## Changes applied to the original source tree
##
#EXTRA_DIST += TLpatches

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

SUBDIRS = .

AM_CPPFLAGS = -I$(top_srcdir)/$(LZ4_TREE) $(LZ4_DEFINES)
AM_CFLAGS = $(WARNING_CFLAGS)

noinst_LIBRARIES = liblz4.a
nodist_liblz4_a_SOURCES = \
	@LZ4_TREE@/lib/lz4.c \
	@LZ4_TREE@/lib/lz4.h \
	@LZ4_TREE@/lib/lz4frame.c \
	@LZ4_TREE@/lib/lz4frame.h \
	@LZ4_TREE@/lib/lz4frame_static.h \
	@LZ4_TREE@/lib/lz4hc.c \
	@LZ4_TREE@/lib/lz4hc.h \
	@LZ4_TREE@/lib/xxhash.c \
	@LZ4_TREE@/lib/xxhash.h
$(liblz4_a_OBJECTS):

LDADD = liblz4.a

# Rebuild
rebuild_prereq =
rebuild_target = all
CLEANFILES =

include $(srcdir)/../../am/rebuild.am

