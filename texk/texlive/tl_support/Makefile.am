## $Id$
## Makefile.am for the TeX Live subdirectory texk/texlive/tl_support/
##
## Copyright 2013-2020 Karl Berry <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.

install-data-hook: install-man1-links
uninstall-hook: uninstall-man1-links

dist_man_MANS = \
  	allcm.1 \
	allec.1 \
	allneeded.1 \
	dvi2fax.1 \
	dvired.1 \
	e2pall.1 \
	fmtutil.1 \
	fmtutil.cnf.5 \
	fontinst.1 \
	kpsetool.1 \
	kpsewhere.1 \
	mktexlsr.1 \
	mktexmf.1 \
	mktexpk.1 \
	mktextfm.1 \
	ps2frag.1 \
	pslatex.1 \
	rubibtex.1 \
	rumakeindex.1 \
	texconfig.1 \
	texlinks.1 \
	updmap.1 \
	updmap.cfg.5

man1_links = \
	fmtutil:fmtutil-sys \
	kpsetool:kpsexpand \
	kpsetool:kpsepath \
	fmtutil:mktexfmt \
	mktexlsr:texhash \
	texconfig:texconfig-sys \
	updmap:updmap-sys

include $(top_srcdir)/../../am/man1_links.am

## Support files
texconfigdir = $(datarootdir)/texmf-dist/texconfig
dist_texconfig_SCRIPTS = tcfmgr
dist_texconfig_DATA = tcfmgr.map

web2cdir = $(datarootdir)/texmf-dist/web2c
dist_web2c_DATA = fmtutil.cnf

## Generate the updmap/fmtutil man page using help2man plus a bit of
## handwritten material.  But don't actually update updmap/fmtutil.1,
## since we don't want to require help2man for an install.  Leave that
## to be done by hand (see tlpkg/doc/releng.txt).
EXTRA_DIST = updmap.help2man fmtutil.help2man
.PHONY: xmans xupdmap.1 xfmtutil.1
xmans: xupdmap.1 xfmtutil.1
Master_dir = $(top_srcdir)/../../../../Master
tl_scripts_dir = $(Master_dir)/texmf-dist/scripts/texlive
#
xupdmap.1: updmap.help2man $(tl_scripts_dir)/updmap.pl
	help2man --include=$< --source="TeX Live" --no-info \
	  $(tl_scripts_dir)/updmap.pl >/tmp/$@
	groff -mandoc -Thtml /tmp/$@ >/tmp/$@.html
#
xfmtutil.1: fmtutil.help2man $(tl_scripts_dir)/fmtutil.pl
	help2man --include=$< --source="TeX Live" --no-info \
	  $(tl_scripts_dir)/fmtutil.pl >/tmp/$@
	groff -mandoc -Thtml /tmp/$@ >/tmp/$@.html

## 
## This target is invoked by the top-level make,
.PHONY: run-texlinks
## to create the per-format engine symlinks in $(bindir),
## after everything else is installed.  We explicitly specify the
## fmtutil.cnf file to use for the sake of distro builds.
## We pass along the executable extension for the sake of cygwin, so we
## don't create dangling symlinks there (even though they work).
##
## The texlinks script is installed by ../linked_scripts/Makefile[.am].
## If it doesn't exist, e.g., if --disable-texlive was given, do nothing here.
## So we do not specify make-level dependencies, but instead check if
## the texlinks file (normally a symlink) and fmtutil.cnf exist.
##
texlinks_prog = $(DESTDIR)$(bindir)/texlinks
texlinks_fmtutil = $(DESTDIR)$(web2cdir)/fmtutil.cnf
#
run_texlinks = $(texlinks_prog) -v \
	-f  $(texlinks_fmtutil) \
	-e "$(EXEEXT)" $(DESTDIR)$(bindir)
#
run-texlinks:
if !WIN32
	if test -s $(texlinks_prog) && test -s $(texlinks_fmtutil); then \
	  echo "running texlinks in `pwd` via: $(run_texlinks)"; \
	  PATH="$(DESTDIR)$(bindir):$(PATH)"; export PATH; $(run_texlinks); \
	else :; fi
endif !WIN32
