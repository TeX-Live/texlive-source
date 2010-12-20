## Makefile.am for the TeX Live subdirectory texk/tetex/
##
## Copyright (C) 2009, 2010 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

dist_bin_SCRIPTS = \
	allcm \
	allneeded \
	dvi2fax \
	dvired \
	fmtutil \
	fmtutil-sys \
	fontinst \
	kpsetool \
	kpsewhere \
	ps2frag \
	pslatex \
	rubibtex \
	rumakeindex \
	texconfig \
	texconfig-dialog \
	texconfig-sys \
	texlinks \
	updmap \
	updmap-sys

dist_man1_MANS = \
	allcm.man \
	allec.man \
	allneeded.man \
	context/ctxtools.man \
	context/mptopdf.man \
	context/pstopdf.man \
	context/texexec.man \
	context/texmfstart.man \
	dvi2fax.man \
	dvired.man \
	e2pall.man \
	fmtutil.man \
	fontinst.man \
	kpsetool.man \
	kpsewhere.man \
	mkjobtexmf.man \
	ps2frag.man \
	pslatex.man \
	rubibtex.man \
	rumakeindex.man \
	texconfig.man \
	texdoctk.man \
	texlinks.man \
	thumbpdf.man \
	updmap.man

dist_man5_MANS = \
	fmtutil.cnf.5 \
	updmap.cfg.5

tetexdocdir = ${prefix}/texmf/doc/tetex

dist_tetexdoc_DATA = \
	doc/TETEXDOC.pdf \
	doc/teTeX-FAQ

texconfdir = ${prefix}

nobase_dist_texconf_DATA = \
	texmf/texconfig/README \
	texmf/texconfig/g/generic \
	texmf/texconfig/generic \
	texmf/texconfig/v/vt100 \
	texmf/texconfig/x/xterm

texconfigdir = ${prefix}/texmf/texconfig

dist_texconfig_SCRIPTS = tcfmgr

dist_texconfig_DATA = tcfmgr.map

web2cdir = ${prefix}/texmf/web2c

dist_web2c_DATA = fmtutil.cnf

# Symlinks within $(bindir): FILE:LINK indicates LINK->FILE
bin_links = \
	allcm:allec \
	fmtutil:mktexfmt \
	kpsetool:kpsexpand \
	kpsetool:kpsepath

# Symlinks within $(man1dir): FILE:LINK indicates LINK.1->FILE.1
man1_links = $(bin_links) \
	fmtutil:fmtutil-sys \
	texconfig:texconfig-sys \
	updmap:updmap-sys

install-exec-hook:
	@cd $(DESTDIR)$(bindir) && \
	  for s in $(bin_links); do \
	    link=`echo $$s | sed 's,.*:,,'`; \
	    file=`echo $$s | sed 's,:.*,,'`; \
	    rm -f $$link; \
	    echo "creating link '$$link' -> '$$file'"; \
	    $(LN_S) $$file $$link; \
	  done

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
	@for s in $(bin_links); do \
	  link=`echo $$s | sed 's,.*:,,'`; \
	  rm -f $(DESTDIR)$(bindir)/$$link; \
	done
	@for s in $(man1_links); do \
	  link=`echo $$s | sed 's,.*:,,'`; \
	  rm -f $(DESTDIR)$(man1dir)/$$link.1; \
	done

EXTRA_DIST = \
	README.a2ping \
	README.texdoctk

## Not used
##
EXTRA_DIST += \
	doc/Makefile \
	doc/TETEXDOC.bib \
	doc/TETEXDOC.tex \
	fontinst.bat \
	pdfetex-pl.pool \
	updmap.pl

## 
## This target is invoked by the top-level make,
## to create the per-format engine symlinks in $(bindir),
## after everything else is installed.  We explicitly specify the
## fmtutil.cnf file to use for the sake of distro builds.
## We pass along the executable extension for the sake of cygwin, so we
## don't create dangling symlinks there (even though they work).
run_texlinks = $(DESTDIR)$(bindir)/texlinks -v \
	-f $(DESTDIR)$(web2cdir)/fmtutil.cnf \
	-e "$(EXEEXT)" $(DESTDIR)$(bindir)
run-texlinks:
	PATH="$(DESTDIR)$(bindir):$(PATH)"; export PATH; $(run_texlinks)

