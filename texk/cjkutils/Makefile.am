##
ACLOCAL_AMFLAGS = -I ../../m4

SUBDIRS = conv hbf2gf scripts

## Not yet used
##
EXTRA_DIST = bat

## in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir)/$(ZLIB_TREE) -name .svn`

## Eventually delete these files
##
EXTRA_DIST += Makefile.in.orig configure.in.orig

