## Makefile.am for the TeX Live subdirectory texk/tetex/
##
## Copyright (C) 2009-2011 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

perl_scripts = \
	updmap

shell_scripts = \
	updmap-sys

sh_scripts = \
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
	texlinks

scriptsdir = texmf/scripts/tetex
nodist_bin_SCRIPTS =

include $(srcdir)/../am/script_links.am

texmfdir = ${prefix}/$(scriptsdir)
dist_texmf_SCRIPTS = $(perl_scripts:=.pl) $(shell_scripts:=.sh)

if !WIN32
dist_bin_SCRIPTS = $(sh_scripts)
endif !WIN32

CLEANFILES = $(nodist_bin_SCRIPTS)

dist_man1_MANS = \
	allcm.man \
	allec.man \
	allneeded.man \
	dvi2fax.man \
	dvired.man \
	e2pall.man \
	fmtutil.man \
	fontinst.man \
	kpsetool.man \
	kpsewhere.man \
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

bin_links = \
	allcm:allec \
	fmtutil:mktexfmt \
	kpsetool:kpsexpand \
	kpsetool:kpsepath

include $(top_srcdir)/../am/bin_links.am

man1_links = $(bin_links) \
	fmtutil:fmtutil-sys \
	texconfig:texconfig-sys \
	updmap:updmap-sys

include $(top_srcdir)/../am/man1_links.am

install-exec-hook: install-bin-links

install-data-hook: install-perl-links install-shell-links install-man1-links

uninstall-hook: uninstall-bin-links uninstall-links uninstall-man1-links

EXTRA_DIST = \
	README.texdoctk

## Not used
##
EXTRA_DIST += \
	fontinst.bat \
	updmap

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

