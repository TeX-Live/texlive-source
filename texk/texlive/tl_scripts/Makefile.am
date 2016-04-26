## Makefile.am for the TeX Live subdirectory texk/texlive/tl_scripts/
##
## Copyright 2013-2015 Karl Berry <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.

## Build lists of scripts.
##
scripts-list: Makefile
	@echo "Create $@"
	@echo "tl_scripts='" >$@
	@for f in $(dist_texmf_SCRIPTS); do echo "$$f"; done >>$@
	@echo "'" >>$@
	cmp -s $(srcdir)/scripts.lst $@ || cp -f $@ $(srcdir)/scripts.lst

if MAINTAINER_MODE
all-local: scripts-list
else !MAINTAINER_MODE
all-local:
endif !MAINTAINER_MODE

DISTCLEANFILES = scripts-list

EXTRA_DIST = scripts.lst

# These first variable definitions are what scripts_links.am requires.
perl_scripts = e2pall
sh_scripts = \
	allcm \
	allneeded \
	dvi2fax \
	dvired \
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
	texlinks
nodist_bin_SCRIPTS =
scriptsdir = texmf-dist/scripts/texlive
#
include $(top_srcdir)/../../am/script_links.am
#
texmfdir = $(datarootdir)/$(scriptsdir)
dist_texmf_SCRIPTS = $(perl_scripts:=.pl) $(sh_scripts:=.sh)

install-data-hook: install-bin-links install-man1-links install-perl-links install-sh-links
uninstall-hook: uninstall-bin-links uninstall-man1-links uninstall-links

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

bin_links = \
	allcm:allec \
	fmtutil:mktexfmt \
	kpsetool:kpsexpand \
	kpsetool:kpsepath

include $(top_srcdir)/../../am/bin_links.am

man1_links = \
	fmtutil:fmtutil-sys \
	kpsetool:kpsexpand \
	kpsetool:kpsepath \
	fmtutil:mktexfmt \
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
## to be done by hand.
EXTRA_DIST += updmap.help2man fmtutil.help2man
.PHONY: xmans xupdmap.1 xfmtutil.1
xmans: xupdmap.1 xfmtutil.1
Master_dir = $(top_srcdir)/../../../../Master
tl_scripts_dir = $(Master_dir)/texmf-dist/scripts/texlive
##
xupdmap.1: updmap.help2man $(tl_scripts_dir)/updmap.pl
	help2man --include=$< --source="TeX Live" --no-info \
	  $(tl_scripts_dir)/updmap.pl >/tmp/$@
##
xfmtutil.1: fmtutil.help2man $(tl_scripts_dir)/fmtutil.pl
	help2man --include=$< --source="TeX Live" --no-info \
	  $(tl_scripts_dir)/fmtutil.pl >/tmp/$@


## 
## This target is invoked by the top-level make,
.PHONY: run-texlinks
## to create the per-format engine symlinks in $(bindir),
## after everything else is installed.  We explicitly specify the
## fmtutil.cnf file to use for the sake of distro builds.
## We pass along the executable extension for the sake of cygwin, so we
## don't create dangling symlinks there (even though they work).
#
texlinks_prog = $(DESTDIR)$(bindir)/texlinks
#
run_texlinks = $(texlinks_prog) -v \
	-f $(DESTDIR)$(web2cdir)/fmtutil.cnf \
	-e "$(EXEEXT)" $(DESTDIR)$(bindir)
#
run-texlinks: $(DESTDIR)$(bindir)/texlinks
if !WIN32
	PATH="$(DESTDIR)$(bindir):$(PATH)"; export PATH; $(run_texlinks)
endif !WIN32
#
$(DESTDIR)$(bindir)/texlinks: $(DESTDIR)$(web2cdir)/fmtutil.cnf
$(DESTDIR)$(web2cdir)/fmtutil.cnf:
	$(MAKE) $(AM_MAKEFLAGS) install

## 
## This target is invoked by check-wrapper-consistency (run from cron).
.PHONY: echo-shell-scripts
echo-shell-scripts:
	@echo $(sh_scripts)
