## Makefile.am for the TeX Live subdirectory texk/dummy/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
noinst_DATA = ../subtexkdir-configure

DISTCLEANFILES = $(noinst_DATA)

all-local: $(noinst_DATA)

../subtexkdir-configure:
	echo "${srcdir}/configure" | sed 's,dummy/,SUBTEXKDIR/,' >$@

