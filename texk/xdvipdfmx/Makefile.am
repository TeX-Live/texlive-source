## Makefile.am for the TeX Live subdirectory texk/dtl/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

if build
SUBDIRS = src
endif build

EXTRA_DIST = \
	BUGS \
	ChangeLog.TL \
	contrib \
	data \
	doc \
	index.html \
	style_base.css \
	style_dflt.css \
	style_ie.css \
	style_plain.css

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`

