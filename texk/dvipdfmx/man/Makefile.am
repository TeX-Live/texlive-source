## Makefile.am for the TeX Live subdirectory texk/dvipdfmx/man/
##
## Copyright (C) 2011 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
# Note:  dvipdft.1 used here needs to be updated
# and dvipdfm.1 be replaced by dvipdfmx.1 with a link to dvipdfm.1
dist_man1_MANS = \
	extractbb.1 \
	dvipdfm.1 \
	dvipdft.1

include $(top_srcdir)/../am/man.am

# Symlinks within $(man1dir): FILE:LINK indicates LINK.1->FILE.1
man1_links = \
	extractbb:ebb

include $(top_srcdir)/../am/man1_links.am

install-data-hook: install-man1-links

uninstall-hook: uninstall-man1-links

