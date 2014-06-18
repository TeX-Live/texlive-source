## Makefile.am for the TeX Live subdirectory texk/dvipsk/squeeze/
##
## Copyright (C) 2009-2014 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
AM_CFLAGS = $(WARNING_CFLAGS)

noinst_PROGRAMS = squeeze

nodist_noinst_DATA = stamp-squeeze

stamp-squeeze: squeeze$(EXEEXT)
	$(AM_V_at)echo timestamp >$@

CLEANFILES = stamp-squeeze

