## Makefile.am for the TeX Live subdirectory texk/
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
recurse_this = texk/
recurse_top = ../

include $(srcdir)/../am/recurse.am

# Common infrastructure for tests
EXTRA_DIST = tests

# Special target to run TRIP and TRAP tests and create diffs.
.PHONY: triptrap
triptrap: web2c/Makefile
	cd web2c && $(MAKE) $(AM_MAKEFLAGS) $@
web2c/Makefile:
	$(MAKE) $(AM_MAKEFLAGS) CONF_SUBDIRS=web2c subtexk
