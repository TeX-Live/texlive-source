## Makefile.am for the TeX Live subdirectory texk/kpathsea/man/
##
## Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
man_sources = \
	kpseaccess.man \
	kpsereadlink.man \
	kpsestat.man \
	kpsewhich.man \
	mktexlsr.man \
	mktexmf.man \
	mktexpk.man \
	mktextfm.man

EXTRA_DIST = $(man_sources)

KPATHSEA_PATHS_H = ${top_builddir}/..

include $(top_srcdir)/../../am/man.am

# Symlinks within $(man1dir): FILE:LINK indicates LINK.1->FILE.1
man1_links = \
	mktexlsr:texhash

include $(top_srcdir)/../../am/man1_links.am

install-data-hook: install-man1-links

uninstall-hook: uninstall-man1-links

