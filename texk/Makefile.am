## Makefile.am for the TeX Live subdirectory texk/
##
## Copyright 2009-2015 Peter Breitenlohner <tex-live@tug.org>
## Copyright 2016 Karl Berry <tex-live@tug.org>
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
triptrap:
	cd web2c && $(MAKE) $(AM_MAKEFLAGS) $@
