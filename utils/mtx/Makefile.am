## Makefile.am for the TeX Live subdirectory utils/mtx/
##
## Copyright (C) 2012 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

## We want to re-distribute the whole original mtx source tree.
EXTRA_DIST = $(MTX_TREE)
## Patches applied to the original source tree
EXTRA_DIST += $(MTX_TREE)-PATCHES

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`

INCLUDES = -I$(srcdir)/$(MTX_TREE)/libp2c
AM_CFLAGS = #$(WARNING_CFLAGS)

bin_PROGRAMS = prepmx

## prepmx_SOURCES =  config.h

prepmx_SOURCES = \
	$(MTX_TREE)/analyze.c \
	$(MTX_TREE)/analyze.h \
	$(MTX_TREE)/cfuncs.c \
	$(MTX_TREE)/cfuncs.h \
	$(MTX_TREE)/control.c \
	$(MTX_TREE)/control.h \
	$(MTX_TREE)/files.c \
	$(MTX_TREE)/files.h \
	$(MTX_TREE)/globals.c \
	$(MTX_TREE)/globals.h \
	$(MTX_TREE)/lyrics.c \
	$(MTX_TREE)/lyrics.h \
	$(MTX_TREE)/mtx.c \
	$(MTX_TREE)/mtx.h \
	$(MTX_TREE)/mtxline.c \
	$(MTX_TREE)/mtxline.h \
	$(MTX_TREE)/multfile.c \
	$(MTX_TREE)/multfile.h \
	$(MTX_TREE)/notes.c \
	$(MTX_TREE)/notes.h \
	$(MTX_TREE)/preamble.c \
	$(MTX_TREE)/preamble.h \
	$(MTX_TREE)/prepmx.c \
	$(MTX_TREE)/status.c \
	$(MTX_TREE)/status.h \
	$(MTX_TREE)/strings.h \
	$(MTX_TREE)/uptext.c \
	$(MTX_TREE)/uptext.h \
	$(MTX_TREE)/utility.c \
	$(MTX_TREE)/utility.h

LDADD = libp2c.a

noinst_LIBRARIES = libp2c.a

libp2c_a_SOURCES = \
	$(MTX_TREE)/libp2c/p2c.h \
	$(MTX_TREE)/libp2c/p2clib.c

lua_scripts = m-tx

scriptsdir = texmf/scripts/m-tx
nodist_bin_SCRIPTS =

include $(srcdir)/../../texk/am/script_links.am

texmfdir = $(datarootdir)/$(scriptsdir)
dist_texmf_SCRIPTS = $(lua_scripts:=.lua)

install-data-hook: install-lua-links

uninstall-hook: uninstall-links

dist_man1_MANS = m-tx.1 prepmx.1

## Tests
##
TESTS = mtx.test
EXTRA_DIST += $(TESTS)

## mtx.test
EXTRA_DIST += tests/mozart.mtx tests/mozart.pmx
DISTCLEANFILES = mozart*

