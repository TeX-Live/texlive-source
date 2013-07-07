## Makefile.am for the TeX Live subdirectory utils/
##
## Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
## Use 'aclocal --force' (for "computed m4_sinclude")
##
ACLOCAL_AMFLAGS = -I ../m4 --force

SUBDIRS = . $(MAKE_SUBDIRS)
DIST_SUBDIRS = $(CONF_SUBDIRS)

cf_silent = $(cf_silent_@AM_V@)
cf_silent_ = $(cf_silent_@AM_DEFAULT_V@)
cf_silent_0 = --silent
cf_silent_1 =

# just in case a subdirectory has added directories
dist-hook:
	rm -rf `find $(distdir) -name .svn`

## Configure and build subdirs.
##
recurse_this = utils/
recurse_top = ../

include $(srcdir)/../am/recurse.am

