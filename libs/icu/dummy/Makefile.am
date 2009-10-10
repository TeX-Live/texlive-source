## Makefile.am for the TeX Live subdirectory libs/icu/dummy/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
noinst_DATA = ../icusubdir-configure

DISTCLEANFILES = $(noinst_DATA)

all-local: $(noinst_DATA)

../icusubdir-configure:
	d=$(srcdir); test "x$$d" = x. && d=../../icu/dummy; \
	echo "$$d/configure" | sed 's,icu/dummy/,icu/icu-xetex/,' >$@

