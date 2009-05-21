## Makefile.am for the TeX Live subdirectory texk/texlive/linked_scripts/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
## TeX Live's extra scripts.

## Make sure $(bindir) exists
##
bin_SCRIPTS =

## Copies of these scripts exist here for the sake of avoiding
## dangling symlinks produced by `make install'.
## The instances in texmf* are the masters (except when it is CTAN).

scriptxdir = ${prefix}/texmf/scripts
nobase_dist_scriptx_SCRIPTS = \
	a2ping/a2ping.pl \
	epstopdf/epstopdf.pl \
	getnonfreefonts/getnonfreefonts.pl \
	ps2eps/ps2eps.pl \
	simpdftex/simpdftex \
	tetex/e2pall.pl \
	tetex/texdoctk.pl \
	texdoc/texdoc.tlu \
	texlive/rungs.tlu \
	texlive/tlmgr.pl

scriptdir = ${prefix}/texmf-dist/scripts
nobase_dist_script_SCRIPTS = \
	accfonts/mkt1font \
	accfonts/vpl2ovp \
	accfonts/vpl2vpl \
	bengali/ebong.py \
	context/lua/luatools.lua \
	context/lua/mtxrun.lua \
	context/ruby/texmfstart.rb \
	context/stubs/unix/context \
	context/stubs/unix/ctxtools \
	context/stubs/unix/exatools \
	context/stubs/unix/makempy \
	context/stubs/unix/mpstools \
	context/stubs/unix/mptopdf \
	context/stubs/unix/mtxtools \
	context/stubs/unix/pdftools \
	context/stubs/unix/pdftrimwhite \
	context/stubs/unix/pstopdf \
	context/stubs/unix/rlxtools \
	context/stubs/unix/runtools \
	context/stubs/unix/texexec \
	context/stubs/unix/texfind \
	context/stubs/unix/texfont \
	context/stubs/unix/texshow \
	context/stubs/unix/textools \
	context/stubs/unix/texutil \
	context/stubs/unix/tmftools \
	context/stubs/unix/xmltools \
	dviasm/dviasm.py \
	epspdf/epspdf \
	epspdf/epspdftk \
	fontools/afm2afm \
	fontools/autoinst \
	fontools/cmap2enc \
	fontools/font2afm \
	fontools/ot2kpx \
	fontools/pfm2kpx \
	fontools/showglyphs \
	fragmaster/fragmaster.pl \
	glossaries/makeglossaries \
	latex2man/latex2man \
	latexmk/latexmk.pl \
	mkjobtexmf/mkjobtexmf.pl \
	oberdiek/pdfatfi.pl \
	pax/pdfannotextractor.pl \
	pdfcrop/pdfcrop.pl \
	perltex/perltex \
	pkfix/pkfix.pl \
	pkfix-helper/pkfix-helper \
	ppower4/pdfthumb.tlu \
	ppower4/ppower4.tlu \
	pst-pdf/ps4pdf \
	pst2pdf/pst2pdf.pl \
	purifyeps/purifyeps \
	splitindex/perl/splitindex.pl \
	svn-multi/svn-multi.pl \
	texcount/texcount.pl \
	texdirflatten/texdirflatten \
	texloganalyser/texloganalyser \
	thumbpdf/thumbpdf.pl \
	ulqda/ulqda.pl \
	vpe/vpe.pl

## The idea is to install the scripts themselves in texmf*/scripts, and
## have bin/arch/foo be a symlink to, say,
## ../../texmf/scripts/foo/foo.pl.  That way we save a bit of disk space,
## but more importantly omit duplication, and most importantly make it
## possible to invoke the same Perl script on Windows (via
## tl-w32-wrapper.texlua).
##
## The installation into texmf*/scripts is ideally done with ctan2tl.
## The copy of the texlua wrapper for Windows is also done by hand, but
## could/should also be done by ctan2tl.  The check-wrapper-consistency
## script at least checks after the fact for problems.
##
## We also keep a copy of all the scripts here and install into the runtime. 
## This is purely for other distro builders, so symlinks are not dangling in
## the inst/bin dir (it is meaningless in native TL).  We have not yet
## written the auto-update to ensure linked_scripts is actually up to date
## with the masters in Master/texmf*/scripts, but it will happen.
##
## We support both multiplatform and non-multiplatform builds.
install-data-hook:
	case "$(bindir)" in \
	  */bin) $(MAKE) $(AM_MAKEFLAGS) REL=.. install-links;; \
	  */bin/*) $(MAKE) $(AM_MAKEFLAGS) REL=../.. install-links;; \
	  *) echo "strange directory '$(bindir)' for linked scripts" >&2; \
	     exit 1;; \
	esac

## Link to the basename, removing any extension,
## downcase for the sake of TeXcount.pl -> texcount.
.PHONY: install-links
install-links:
	@cd $(DESTDIR)$(bindir) && \
	  for s in $(nobase_dist_scriptx_SCRIPTS); do \
	    target=`basename $$s | sed 's,\.[^/]*$$,,' | tr '[A-Z]' '[a-z]'`; \
	    target=$$target; \
	    rm -f $$target; \
	    echo "creating link '$$target' -> '$(REL)/texmf/scripts/$$s'"; \
	    $(LN_S) $(REL)/texmf/scripts/$$s $$target; \
	  done && \
	  for s in $(nobase_dist_script_SCRIPTS); do \
	    target=`basename $$s | sed 's,\.[^/]*$$,,' | tr '[A-Z]' '[a-z]'`; \
	    target=$$target; \
	    rm -f $$target; \
	    echo "creating link '$$target' -> '$(REL)/texmf-dist/scripts/$$s'"; \
	    $(LN_S) $(REL)/texmf-dist/scripts/$$s $$target; \
	  done && \
	  rm -rf getnonfreefonts-sys && \
	  echo "creating link 'getnonfreefonts-sys' -> 'getnonfreefonts'" && \
	  $(LN_S) getnonfreefonts getnonfreefonts-sys && \
## man dir link for those mans which can use it.
	  rm -f man && \
	  echo "creating link 'man' -> '$(REL)/texmf/doc/man'" && \
	  (ln -s $(REL)/texmf/doc/man man || :)

uninstall-hook:
	@for s in $(nobase_dist_scriptx_SCRIPTS) $(nobase_dist_script_SCRIPTS); do \
	  target=`basename $$s | sed 's,\.[^/]*$$,,' | tr '[A-Z]' '[a-z]'`; \
	  target=$$target; \
	  rm -f $(DESTDIR)$(bindir)/$$target; \
	done
	rm -f $(DESTDIR)$(bindir)/getnonfreefonts-sys $(DESTDIR)$(bindir)/man

