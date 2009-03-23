##
ACLOCAL_AMFLAGS = -I ../../m4 -I m4

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
	m4/cho-kpse.m4 \
	style_base.css \
	style_dflt.css \
	style_ie.css \
	style_plain.css

## in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir)/$(ZLIB_TREE) -name .svn`

## Eventually delete these files
##
EXTRA_DIST += Makefile.am.orig configure.in.orig

