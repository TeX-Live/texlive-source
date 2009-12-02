## Makefile.am for the TeX Live subdirectory texk/dvipng/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

EXTRA_DIST = README-clisp

## We use the original xindy source tree.
##
SUBDIRS = . xindy

## Patches applied to the original source tree
##
EXTRA_DIST +=  xindy-$(VERSION)-PATCHES

## We must explicitly distribute some top-level files.
##
EXTRA_DIST += \
	xindy/AUTHORS \
	xindy/COPYING \
	xindy/INSTALL \
	xindy/Makefile.am \
	xindy/README \
	xindy/NEWS \
	xindy/m4 \
	xindy/configure.ac

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`

all-local: config.force

config.force:
	echo timestamp >config.force
	$(SHELL) ./config.status --recheck

