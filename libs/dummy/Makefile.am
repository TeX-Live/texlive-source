## Makefile.am for the TeX Live subdirectory libs/dummy/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
noinst_DATA = ../sublibsdir-configure

DISTCLEANFILES = $(noinst_DATA)

all-local: $(noinst_DATA)

../sublibsdir-configure:
	echo "${srcdir}/configure" | sed 's,dummy/,SUBLIBSDIR/,' >$@

