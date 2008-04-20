# $Id: Makefile.am,v 1.34 2008/03/25 10:51:20 akim Exp $
# Makefile.am for texinfo/util.
# Run automake in .. to produce Makefile.in from this.
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

bin_PROGRAMS = texindex
bin_SCRIPTS = texi2dvi texi2pdf pdftexi2dvi

# for auctex.
pdftexi2dvi: texi2pdf
	cp -f $(srcdir)/texi2pdf $@

localedir = $(datadir)/locale
AM_CPPFLAGS = 					\
  -I$(top_srcdir)				\
  -I$(top_srcdir)/gnulib/lib			\
  -I$(top_builddir)/gnulib/lib			\
  -DLOCALEDIR=\"$(localedir)\"
LDADD = ../lib/libtxi.a $(top_builddir)/gnulib/lib/libgnu.a $(LIBINTL)

dist_pkgdata_DATA = texinfo.dtd texinfo.xsl
pkgdata_DATA = texinfo.cat

# we don't actually use version.texi in the rule, but it's one way of
# noticing when the version changes.
texinfo.cat: texinfo-cat.in $(top_srcdir)/doc/version.texi
	sed 's/__VERSION__/@VERSION@/g' $(srcdir)/texinfo-cat.in >$@

# Most of these are for fun.  The only official/installed ones are the
# *texi2* scripts.
#
EXTRA_DIST = README deref.c dir-example fix-info-dir fixfonts \
  fixref.gawk gdoc gen-dir-node gendocs.sh gendocs_template infosrch \
  install-info-html outline.gawk pdftexi2dvi \
  prepinfo.awk tex3patch texi-docstring-magic.el texi2dvi texi2pdf txitextest \
  $(dist_pkgdata_DATA) texinfo-cat.in
CLEANFILES = $(pkgdata_DATA)


# Let's not stress people's TeX installations, etc.
if MAINTAINER_MODE
TESTS =						\
bibtex.test					\
dvipdf.test					\
latex2html.test					\
local.test					\
texi2dvi.test

EXTRA_DIST += $(TESTS)

# Each test case depends on defs.
check_SCRIPTS = defs
distclean-local:
	-rm -rf testSubDir
endif
