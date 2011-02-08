## Makefile.am for the TeX Live subdirectory texk/tetex/
##
## Copyright (C) 2009 - 2011 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

shell_scripts = \
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

## Handled as alias by runscript.tlu
win32_aliases = \
	fontinst.exe

if WIN32
if WIN32_WRAP
## We treat the WIN32 aliases as SCRIPTS to avoid automatic build rules
nodist_bin_SCRIPTS = $(win32_aliases)
runscript = $(srcdir)/../texlive/w32_wrapper/runscript.exe
$(win32_aliases): $(runscript)
	$(LN_S) $(runscript) $@
endif WIN32_WRAP
else !WIN32
nodist_bin_SCRIPTS = $(shell_scripts)
endif !WIN32

EXTRA_DIST = $(shell_scripts)
CLEANFILES = $(win32_aliases)

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

include $(top_srcdir)/../am/man1_links.am

install-exec-hook:
if !WIN32
	@cd $(DESTDIR)$(bindir) && \
	  for s in $(bin_links); do \
	    link=`echo $$s | sed 's,.*:,,'`; \
	    file=`echo $$s | sed 's,:.*,,'`; \
	    rm -f $$link; \
	    echo "creating link '$$link' -> '$$file'"; \
	    $(LN_S) $$file $$link || exit 1; \
	  done
endif !WIN32

install-data-hook: install-man1-links

uninstall-hook: uninstall-man1-links
if !WIN32
	@for s in $(bin_links); do \
	  link=`echo $$s | sed 's,.*:,,'`; \
	  rm -f $(DESTDIR)$(bindir)/$$link; \
	done
endif !WIN32

EXTRA_DIST += \
	README.texdoctk

## Not used
##
EXTRA_DIST += \
	fontinst.bat \
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

