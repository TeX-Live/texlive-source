## Proxy Makefile.am to build dialog for TeX Live.
##
##   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
ACLOCAL_AMFLAGS = -I ../../m4

## The dialog-1.1-YYYYMMDD build system is good to build the binary,
## but lacks many required targets.
##
## DIALOG_TREE is the unmodified source tree,
## DIALOG_BUILD is the build tree (renamed after configure).

## install dialog as tcdialog
program_transform_name = 's,^,tc,'

## We want to re-distribute the whole original dialog source tree.
##
## With current automake (1.10.2) 'make distcheck' fails when
## DISTFILES contains a directory and files in that directory.
## Thus nodist_* for all files in $(DIALOG_TREE).
EXTRA_DIST = $(DIALOG_TREE)

## in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir)/$(ZLIB_TREE) -name .svn`

if build

nodist_man1_MANS = $(DIALOG_TREE)/dialog.1

bin_PROGRAMS = $(DIALOG_BUILD)/dialog

$(DIALOG_BUILD)/dialog$(EXEEXT):
	cd $(DIALOG_BUILD) && $(MAKE) $(AM_MAKEFLAGS) all

distclean-local:
	cd $(DIALOG_BUILD) && $(MAKE) $(AM_MAKEFLAGS) distclean

else !build
distclean-local:
endif !build

__DIALOG_BUILD__dialog_SOURCES =

