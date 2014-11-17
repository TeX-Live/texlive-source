## Makefile.am for the TeX Live subdirectory texk/texlive/linked_scripts/
##
## Copyright (C) 2009-2014 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
## TeX Live's extra scripts.

## Build lists of texmf and texmf-dist scripts.
##
scripts-list: Makefile
	@echo "Create $@"
	@echo "texmf_scripts='" >$@
	@for f in $(nobase_dist_texmf_scripts_SCRIPTS); do echo "$$f"; done >>$@
	@echo "'" >>$@
	cmp -s $(srcdir)/scripts.lst $@ || cp -f $@ $(srcdir)/scripts.lst

if MAINTAINER_MODE
all-local: scripts-list
else !MAINTAINER_MODE
all-local:
endif !MAINTAINER_MODE

DISTCLEANFILES = scripts-list

EXTRA_DIST = scripts.lst

## Make sure $(bindir) exists
##
installdirs-local:
	$(MKDIR_P) $(DESTDIR)$(bindir)

# Copies of these scripts exist here for the sake of avoiding
# dangling symlinks produced by `make install'.
# The instances in texmf* are the masters (except when it is CTAN).
#
## We distinguish several classes of scripts according to their
## type: `shell', `context', or `other'.
## 
## On Unix systems a symlink from $(bindir) to the script is installed
## for all of them; on Windows nothing is installed in $(bindir) for
## `shell' scripts whereas for `other' (usually perl, lua, ...) scripts
## the standard wrapper ../$(WIN_WRAPPER)/runscript.exe is installed or
## the ConTeXt wrapper ../$(WIN_WRAPPER)/context/mtxrun.exe for
## `context' scripts.
##
## WIN_WRAPPER is w32_wrapper for 32-bit or w64_wrapper for 64-bit.
## 
## Some /bin/sh scripts are listed as `other' or `context'
## scripts because they are correctly handled by the wrapper binaries.
## 
texmf_scriptsdir = $(datarootdir)/texmf-dist/scripts
texmf_shell_scripts = \
	adhocfilelist/adhocfilelist.sh \
	bibexport/bibexport.sh \
	dtxgen/dtxgen \
	installfont/installfont-tl \
	latexfileversion/latexfileversion \
	listbib/listbib \
	listings-ext/listings-ext.sh \
	ltxfileinfo/ltxfileinfo \
	lua2dox/lua2dox_filter \
	pdfjam/pdf180 \
	pdfjam/pdf270 \
	pdfjam/pdf90 \
	pdfjam/pdfbook \
	pdfjam/pdfflip \
	pdfjam/pdfjam \
	pdfjam/pdfjam-pocketmod \
	pdfjam/pdfjam-slides3up \
	pdfjam/pdfjam-slides6up \
	pdfjam/pdfjoin \
	pdfjam/pdfnup \
	pdfjam/pdfpun \
	pst-pdf/ps4pdf \
	simpdftex/simpdftex \
	typeoutfileinfo/typeoutfileinfo.sh
texmf_other_scripts = \
	a2ping/a2ping.pl \
	accfonts/mkt1font \
	accfonts/vpl2ovp \
	accfonts/vpl2vpl \
	arara/arara.sh \
	authorindex/authorindex \
	bundledoc/arlatex \
	bundledoc/bundledoc \
	cachepic/cachepic.tlu \
	checkcites/checkcites.lua \
	context/perl/mptopdf.pl \
	convbkmk/convbkmk.rb \
	crossrefware/bibdoiadd.pl \
	crossrefware/bibzbladd.pl \
	crossrefware/ltx2crossrefxml.pl \
	ctanify/ctanify \
	ctanupload/ctanupload.pl \
	de-macro/de-macro \
	dosepsbin/dosepsbin.pl \
	dviasm/dviasm.py \
	ebong/ebong.py \
	epspdf/epspdf.tlu \
	epspdf/epspdftk.tcl \
	epstopdf/epstopdf.pl \
	exceltex/exceltex \
	fig4latex/fig4latex \
	findhyph/findhyph \
	fontools/afm2afm \
	fontools/autoinst \
	fontools/ot2kpx \
	fragmaster/fragmaster.pl \
	getmap/getmapdl.lua \
	glossaries/makeglossaries \
	jfontmaps/kanji-fontmap-creator.pl \
	jfontmaps/kanji-config-updmap.pl \
	jfontmaps/kanji-config-updmap-sys.sh \
	kotex-utils/jamo-normalize.pl \
	kotex-utils/komkindex.pl \
	kotex-utils/ttf2kotexfont.pl \
	latex-git-log/latex-git-log \
	latex2man/latex2man \
	latexdiff/latexdiff.pl \
	latexdiff/latexdiff-vc.pl \
	latexdiff/latexrevise.pl \
	latexindent/latexindent.pl \
	latexmk/latexmk.pl \
	latexpand/latexpand \
	lilyglyphs/lily-glyph-commands.py \
	lilyglyphs/lily-image-commands.py \
	lilyglyphs/lily-rebuild-pdfs.py \
	ltximg/ltximg.pl \
	luaotfload/luaotfload-tool.lua \
	m-tx/m-tx.lua \
	match_parens/match_parens \
	mathspic/mathspic.pl \
	mf2pt1/mf2pt1.pl \
	mkjobtexmf/mkjobtexmf.pl \
	mkgrkindex/mkgrkindex \
	mkpic/mkpic \
	multibibliography/multibibliography.pl \
	musixtex/musixflx.lua \
	musixtex/musixtex.lua \
	oberdiek/pdfatfi.pl \
	pax/pdfannotextractor.pl \
	pdfcrop/pdfcrop.pl \
	pedigree-perl/pedigree.pl \
	perltex/perltex.pl \
	pfarrei/pfarrei.tlu \
	pfarrei/a5toa4.tlu \
	pkfix/pkfix.pl \
	pkfix-helper/pkfix-helper \
	pmx/pmx2pdf.lua \
	pmxchords/pmxchords.lua \
	pst2pdf/pst2pdf.pl \
	ptex2pdf/ptex2pdf.lua \
	purifyeps/purifyeps \
	pygmentex/pygmentex.py \
	pythontex/pythontex.py \
	pythontex/depythontex.py \
	rubik/rubikrotation.pl \
	splitindex/splitindex.pl \
	sty2dtx/sty2dtx.pl \
	svn-multi/svn-multi.pl \
	texdoc/texdoc.tlu \
	texcount/texcount.pl \
	texdef/texdef.pl \
	texdiff/texdiff \
	texdirflatten/texdirflatten \
	texfot/texfot.pl \
	texlive/rungs.tlu \
	texlive/tlmgr.pl \
	texlive/updmap.pl \
	texliveonfly/texliveonfly.py \
	texloganalyser/texloganalyser \
	thumbpdf/thumbpdf.pl \
	ulqda/ulqda.pl \
	urlbst/urlbst \
	vpe/vpe.pl \
	yplan/yplan
texmf_context_scripts = \
	context/stubs/unix/context \
	context/stubs/unix/contextjit \
	context/stubs/unix/luatools \
	context/stubs/unix/mtxrun \
	context/stubs/unix/mtxrunjit \
	context/stubs/unix/texexec \
	context/stubs/unix/texmfstart

EXTRA_DIST += \
	context/stubs/unix/ctxtools \
	context/stubs/unix/pstopdf

nobase_dist_texmf_scripts_SCRIPTS = \
	$(texmf_shell_scripts) \
	$(texmf_other_scripts) \
	$(texmf_context_scripts)

## Symlinks within $(bindir): FILE:LINK indicates LINK->FILE
bin_links = \
	epstopdf:repstopdf \
	pdfcrop:rpdfcrop

# The idea is to install the scripts themselves in texmf*/scripts, and
# have bin/arch/foo be a symlink to, say,
# ../../texmf-dist/scripts/foo/foo.pl.  That way we save a bit of disk
# space, but more importantly omit duplication, and most
# importantly of all make it possible to invoke the same Perl script on
# Windows (see ../$(WIN_WRAPPER)).
#
# The installation into texmf*/scripts is ideally done with ctan2tl.
# The copy of the wrapper for Windows is also done by ctan2tl.  (And
# for shell scripts, its addition must be undone by hand.)  The
# check-wrapper-consistency script checks after the fact for problems.
#
# We also keep a copy of all the scripts here and install into the runtime.
# This is purely for other distro builders, so symlinks are not dangling in
# the inst/bin dir (it is meaningless in native TL).  We have not yet
# written the auto-update to ensure linked_scripts is actually up to date
# with the masters in Master/texmf*/scripts, but one day.
#
# We support native builds, multiplatform or not, as well as distro builds.
#
EXTRA_DIST += ../../../build-aux/relpath
relpath = $(SHELL) $(top_srcdir)/../../build-aux/relpath
runscript = $(top_srcdir)/$(WIN_WRAPPER)/runscript.exe
ctxscript = $(top_srcdir)/$(WIN_WRAPPER)/context/mtxrun.exe
install-data-hook: installdirs-local
if WIN32
	@for s in $(texmf_other_scripts); do \
	  t=`basename $$s | sed 's,\.[^/]*$$,,' | tr '[A-Z]' '[a-z]'`; \
	  echo "$(INSTALL_SCRIPT) '$(runscript)' '$(DESTDIR)$(bindir)/$$t.exe'"; \
	  $(INSTALL_SCRIPT) $(runscript) $(DESTDIR)$(bindir)/$$t.exe; \
	done
	$(INSTALL_SCRIPT) $(top_srcdir)/$(WIN_WRAPPER)/runscript.dll $(DESTDIR)$(bindir)
	$(INSTALL_SCRIPT) $(top_srcdir)/$(WIN_WRAPPER)/runscript.exe $(DESTDIR)$(bindir)
	$(INSTALL_SCRIPT) $(top_srcdir)/$(WIN_WRAPPER)/runscript.tlu $(DESTDIR)$(bindir)
	@for s in $(texmf_context_scripts); do \
	  t=`basename $$s | sed 's,\.[^/]*$$,,' | tr '[A-Z]' '[a-z]'`; \
	  echo "$(INSTALL_SCRIPT) '$(ctxscript)' '$(DESTDIR)$(bindir)/$$t.exe'"; \
	  $(INSTALL_SCRIPT) $(ctxscript) $(DESTDIR)$(bindir)/$$t.exe; \
	done
	$(INSTALL_SCRIPT) $(top_srcdir)/$(WIN_WRAPPER)/context/mtxrun.dll $(DESTDIR)$(bindir)
	$(INSTALL_SCRIPT) $(srcdir)/context/stubs/unix/mtxrun $(DESTDIR)$(bindir)/mtxrun.lua
	@for s in $(bin_links); do \
	  link=`echo $$s | sed 's,.*:,,'`; \
	  echo "$(INSTALL_SCRIPT) '$(runscript)' '$(DESTDIR)$(bindir)/$$link.exe'"; \
	  $(INSTALL_SCRIPT) $(runscript) $(DESTDIR)$(bindir)/$$link.exe || exit 1; \
	done
else !WIN32
## Name the link as the basename, removing any extension,
## except for listings-ext.sh, which is documented as that name.
## Downcase for the sake of TeXcount.pl -> texcount.
	@REL=`$(relpath) '$(DESTDIR)' '$(bindir)' '$(datarootdir)'`; \
	if test -z "$$REL"; then \
	  echo 'unable to compute relative path for linked scripts' >&2; \
	  exit 1; \
	fi; \
	cd $(DESTDIR)$(bindir) && \
	  for s in $(nobase_dist_texmf_scripts_SCRIPTS); do \
	    target=`basename $$s | tr '[A-Z]' '[a-z]'`; \
	    echo "$$s" | grep listings-ext.sh >/dev/null \
	    || target=`echo $$target | sed 's,\.[^/]*$$,,'`; \
	    rm -f $$target; \
	    echo "creating link '$$target' -> '$$REL/texmf-dist/scripts/$$s'"; \
	    $(LN_S) $$REL/texmf-dist/scripts/$$s $$target || exit 1; \
	  done && \
	  for s in $(bin_links); do \
	    link=`echo $$s | sed 's,.*:,,'`; \
	    file=`echo $$s | sed 's,:.*,,'`; \
	    rm -f $$link; \
	    echo "creating link '$$link' -> '$$file'"; \
	    $(LN_S) $$file $$link || exit 1; \
	  done && \
## man dir link for those mans which can use it, but don't overwrite a
## real file, e.g., /usr/bin/man.
	  if test -d "$$REL/texmf-dist/doc/man" \
	     && test -z "$(TL_INSTALL_OMIT_MAN_LINK)" \
	     && (test -h man || test ! -e man); then \
	    echo "creating link 'man' -> '$$REL/texmf-dist/doc/man'"; \
	    rm -f man; \
	    (ln -s "$$REL/texmf-dist/doc/man" man || :); \
	  else :; fi
endif !WIN32

.PHONY: install-links
install-links:

## For WIN32 (MinGW or native) we have wrapper binaries with .exe,
## for Cygwin and Unix we have symlinks without .exe.
if WIN32
WINEXT = $(EXEEXT)
endif WIN32

uninstall-hook:
	@for s in $(nobase_dist_texmf_scripts_SCRIPTS); do \
	  target=`basename $$s | tr '[A-Z]' '[a-z]'`; \
	  echo "$$s" | grep listings-ext.sh >/dev/null \
	  || target=`echo $$target | sed 's,\.[^/]*$$,,'`; \
	  rm -f $(DESTDIR)$(bindir)/$$target$(WINEXT); \
	done
	@for s in $(bin_links); do \
	  link=`echo $$s | sed 's,.*:,,'`; \
	  rm -f $(DESTDIR)$(bindir)/$$link$(WINEXT); \
	done
if !WIN32
	if test -h $(DESTDIR)$(bindir)/man; then \
	  rm -f $(DESTDIR)$(bindir)/man; \
	else :; fi
endif !WIN32

## 
## This target is invoked by check-wrapper-consistency (run from cron).
.PHONY: echo-shell-scripts
shell_scripts = $(texmf_shell_scripts) $(texmf_context_scripts)
echo-shell-scripts:
	@echo $(shell_scripts)
