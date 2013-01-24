## Makefile.am for the TeX Live subdirectory texk/texlive/tl_scripts/
##
## Copyright (C) 2013 Karl Berry <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.

EXTRA_DIST = README
EXTRA_DIST += ../../../build-aux/relpath

# These first variable definitions are what scripts_links.am requires.
sh_scripts = \
	allcm \
	allneeded \
	dvi2fax \
	dvired \
	fmtutil-sys \
	fmtutil \
	fontinst \
	kpsetool \
	kpsewhere \
	ps2frag \
	pslatex \
	rubibtex \
	rumakeindex \
	texconfig-dialog \
	texconfig-sys \
	texconfig \
	texlinks \
	updmap-sys
perl_scripts = updmap e2pall
nodist_bin_SCRIPTS =
scriptsdir = texmf-dist/scripts/texlive
#
include $(srcdir)/../../../texk/am/script_links.am
#
texmfdir = $(datarootdir)/$(scriptsdir)
dist_texmf_SCRIPTS = $(perl_scripts:=.pl) $(shell_scripts:=.sh)

install-data-hook: install-perl-links install-sh-links
uninstall-hook: uninstall-links

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
	ps2frag.1 \
	pslatex.1 \
	rubibtex.1 \
	rumakeindex.1 \
	texconfig.1 \
	texlinks.1 \
	updmap.1 \
	updmap.cfg.5

#if !WIN32
#bin_links += \
#	allcm:allec \
#	fmtutil:mktexfmt \
#	kpsetool:kpsexpand \
#	kpsetool:kpsepath \
#endif !WIN32

man1_links = \
	fmtutil:fmtutil-sys \
	kpsetool:kpsexpand \
	kpsetool:kpsepath \
	fmtutil:mktexfmt \
	texconfig:texconfig-sys \
	updmap:updmap-sys

include $(top_srcdir)/../am/man1_links.am

## Support files
texconfigdir = $(datarootdir)/texmf/texconfig
dist_texconfig_SCRIPTS = tcfmgr
dist_texconfig_DATA = tcfmgr.map

web2cdir = $(datarootdir)/texmf/web2c
dist_web2c_DATA = fmtutil.cnf

## Generate the updmap man page using help2man plus a bit of
## handwritten material.  But don't actually update updmap.1,
## we don't want to require help2man for an install.  Leave that to be
## done by hand.
.PHONY: xupdmap.1
xupdmap.1: updmap.help2man updmap.pl
	help2man --include=$< --source="TeX Live" --no-info \
	  $(srcdir)/updmap.pl >/tmp/$@
EXTRA_DIST += updmap.help2man


## 
## This target is invoked by the top-level make,
.PHONY: run-texlinks
## to create the per-format engine symlinks in $(bindir),
## after everything else is installed.  We explicitly specify the
## fmtutil.cnf file to use for the sake of distro builds.
## We pass along the executable extension for the sake of cygwin, so we
## don't create dangling symlinks there (even though they work).
run_texlinks = $(DESTDIR)$(bindir)/texlinks -v \
	-f $(DESTDIR)$(web2cdir)/fmtutil.cnf \
	-e "$(EXEEXT)" $(DESTDIR)$(bindir)
run-texlinks:
if !WIN32
	PATH="$(DESTDIR)$(bindir):$(PATH)"; export PATH; $(run_texlinks)
endif !WIN32

