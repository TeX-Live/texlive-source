## Makefile.am for the TeX Live subdirectory texk/cjkutils/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

SUBDIRS = conv hbf2gf scripts

## Not used
##
EXTRA_DIST = bat

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`

