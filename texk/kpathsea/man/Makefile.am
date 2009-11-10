## Makefile.am for the TeX Live subdirectory texk/kpathsea/man/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
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

include $(top_srcdir)/../am/man.am

# Symlinks within $(man1dir): FILE:LINK indicates LINK.1->FILE.1
man1_links = \
	mktexlsr:texhash

install-data-hook:
	@cd $(DESTDIR)$(man1dir) && \
	  for s in $(man1_links); do \
	    link=`echo $$s | sed 's,.*:,,'`; \
	    file=`echo $$s | sed 's,:.*,,'`; \
	    rm -f $$link.1; \
	    echo "creating link '$$link.1' -> '$$file.1'"; \
	    $(LN_S) $$file.1 $$link.1; \
	  done

uninstall-hook:
	@for s in $(man1_links); do \
	  link=`echo $$s | sed 's,.*:,,'`; \
	  rm -f $(DESTDIR)$(man1dir)/$$link.1; \
	done

