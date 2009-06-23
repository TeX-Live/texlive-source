## Makefile.am for the TeX Live subdirectory texk/tetex/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
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
	ps4pdf \
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
	context/makempy.man \
	context/mptopdf.man \
	context/pdftools.man \
	context/pstopdf.man \
	context/texexec.man \
	context/texfind.man \
	context/texfont.man \
	context/texmfstart.man \
	context/textools.man \
	context/texutil.man \
	dvi2fax.man \
	dvired.man \
	e2pall.man \
	epstopdf.man \
	fmtutil.man \
	fontinst.man \
	kpsetool.man \
	kpsewhere.man \
	mkjobtexmf.man \
	mkocp.man \
	mkofm.man \
	ofm2opl.man \
	opl2ofm.man \
	otp2ocp.man \
	outocp.man \
	ovf2ovp.man \
	ovp2ovf.man \
	ps2frag.man \
	pslatex.man \
	rubibtex.man \
	rumakeindex.man \
	texconfig.man \
	texdoctk.man \
	texlinks.man \
	thumbpdf.man \
	ttf2afm.man \
	updmap.man

dist_man5_MANS = \
	fmtutil.cnf.5 \
	updmap.cfg.5

tetexdocdir = ${prefix}/texmf/doc/tetex

dist_tetexdoc_DATA = \
	doc/TETEXDOC.dvi \
	doc/TETEXDOC.pdf \
	doc/TETEXDOC.ps \
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

install-exec-hook:
	cd $(DESTDIR)$(bindir) \
	  && rm -f allec kpsexpand kpsepath mktexfmt texhash \
	  && $(LN_S) allcm allec \
	  && $(LN_S) fmtutil mktexfmt \
	  && $(LN_S) kpsetool kpsexpand \
	  && $(LN_S) kpsetool kpsepath \
	  && $(LN_S) mktexlsr texhash

install-data-hook:
	cd $(DESTDIR)$(man1dir) \
	  && rm -f allec.1 ctangle.1 cweave.1 fmtutil-sys.1 kpsexpand.1 \
	    kpsepath.1 lambda.1 mf-nowin.1 mktexfmt.1 odvicopy.1 \
	    odvitype.1 pdflatex.1 texconfig-sys.1 texhash.1 updmap-sys.1 \
	  && $(LN_S) aleph.1 lambda.1 \
	  && $(LN_S) allcm.1 allec.1 \
	  && $(LN_S) cweb.1 ctangle.1 \
	  && $(LN_S) cweb.1 cweave.1 \
	  && $(LN_S) dvicopy.1 odvicopy.1 \
	  && $(LN_S) dvitype.1 odvitype.1 \
	  && $(LN_S) fmtutil.1 fmtutil-sys.1 \
	  && $(LN_S) fmtutil.1 mktexfmt.1 \
	  && $(LN_S) kpsetool.1 kpsexpand.1 \
	  && $(LN_S) kpsetool.1 kpsepath.1 \
	  && $(LN_S) mf.1 mf-nowin.1 \
	  && $(LN_S) mktexlsr.1 texhash.1  \
	  && $(LN_S) pdftex.1 pdflatex.1 \
	  && $(LN_S) texconfig.1 texconfig-sys.1 \
	  && $(LN_S) updmap.1 updmap-sys.1

uninstall-hook:
	cd $(DESTDIR)$(bindir) \
	  && rm -f allec kpsexpand kpsepath mktexfmt texhash
	cd $(DESTDIR)$(man1dir) \
	  && rm -f allec.1 ctangle.1 cweave.1 fmtutil-sys.1 kpsexpand.1 \
	    kpsepath.1 lambda.1 mf-nowin.1 mktexfmt.1 odvicopy.1 \
	    odvitype.1 pdflatex.1 texconfig-sys.1 texhash.1 updmap-sys.1

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
	updmap.pl \
	win32/epstopdf.c \
	win32/epstopdf.txt

## 
## This target is invoked by the top-level make,
## to create the per-format engine symlinks in $(bindir),
## after everything else is installed.  We explicitly specify the
## fmtutil.cnf file to use for the sake of distro builds.
## We pass along the executable extension for the sake of cygwin, so we
## don't create dangling symlinks there (even though they work).
run_texlinks = $(bindir)/texlinks -v -f $(web2cdir)/fmtutil.cnf -e "$(EXEEXT)"
run-texlinks:
	PATH="$(bindir):$(PATH)"; export PATH; $(run_texlinks)

