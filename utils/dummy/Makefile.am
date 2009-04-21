## Makefile.am for the TeX Live subdirectory utils/dummy/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
noinst_DATA = ../subutilsdir-configure

DISTCLEANFILES = $(noinst_DATA)

all-local: $(noinst_DATA)

../subutilsdir-configure:
	echo "${srcdir}/configure" | sed 's,dummy/,SUBUTILSDIR/,' >$@

