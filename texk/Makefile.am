## Makefile.am for the TeX Live subdirectory texk/
##
## Copyright (C) 2009-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
## Ensure aclocal.m4 is up to date
ACLOCAL = @ACLOCAL@ --force

SUBDIRS = . $(MAKE_SUBDIRS)
DIST_SUBDIRS = $(CONF_SUBDIRS)

# Files not to be distributed
include $(srcdir)/../am/dist_hook.am

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
