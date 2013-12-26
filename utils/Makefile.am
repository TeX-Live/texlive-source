## Makefile.am for the TeX Live subdirectory utils/
##
## Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
## Ensure aclocal.m4 is up to date
ACLOCAL = @ACLOCAL@ --force

SUBDIRS = . $(MAKE_SUBDIRS)
DIST_SUBDIRS = $(CONF_SUBDIRS)

# just in case a subdirectory has added directories
dist-hook:
	rm -rf `find $(distdir) -name .svn`

## Configure and build subdirs.
##
recurse_this = utils/
recurse_top = ../

include $(srcdir)/../am/recurse.am

