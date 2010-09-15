## Makefile.am for the TeX Live subdirectory texk/texlive/linked_scripts/
##
## Copyright (C) 2009, 2010 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
## TeX Live's extra scripts.

## Make sure $(bindir) exists
##
bin_SCRIPTS =

# Copies of these scripts exist here for the sake of avoiding
# dangling symlinks produced by `make install'.
# The instances in texmf* are the masters (except when it is CTAN).
#
scriptxdir = ${prefix}/texmf/scripts
nobase_dist_scriptx_SCRIPTS = \
	a2ping/a2ping.pl \
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
	authorindex/authorindex \
	bengali/ebong.py \
	bibexport/bibexport.sh \
	bundledoc/arlatex \
	bundledoc/bundledoc \
	cachepic/cachepic.tlu \
	context/perl/mptopdf.pl \
	de-macro/de-macro \
	dviasm/dviasm.py \
	epspdf/epspdf \
	epspdf/epspdftk \
	epstopdf/epstopdf.pl \
	fig4latex/fig4latex \
	findhyph/findhyph \
	fontools/afm2afm \
	fontools/autoinst \
	fontools/cmap2enc \
	fontools/font2afm \
	fontools/ot2kpx \
	fontools/pfm2kpx \
	fontools/showglyphs \
	fragmaster/fragmaster.pl \
	glossaries/makeglossaries \
	installfont/installfont-tl \
	latex2man/latex2man \
	latexdiff/latexdiff.pl \
	latexdiff/latexdiff-vc.pl \
	latexdiff/latexrevise.pl \
	latexmk/latexmk.pl \
	luaotfload/mkluatexfontdb.lua \
	listings-ext/listings-ext.sh \
	mathspic/mathspic.pl \
	mkjobtexmf/mkjobtexmf.pl \
	mkgrkindex/mkgrkindex \
	oberdiek/pdfatfi.pl \
	pax/pdfannotextractor.pl \
	pdfcrop/pdfcrop.pl \
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
	perltex/perltex.pl \
	pkfix/pkfix.pl \
	pkfix-helper/pkfix-helper \
	ppower4/pdfthumb.tlu \
	ppower4/ppower4.tlu \
	pst-pdf/ps4pdf \
	pst2pdf/pst2pdf \
	purifyeps/purifyeps \
	splitindex/perl/splitindex.pl \
	svn-multi/svn-multi.pl \
	texcount/texcount.pl \
	texdiff/texdiff \
	texdirflatten/texdirflatten \
	texloganalyser/texloganalyser \
	thumbpdf/thumbpdf.pl \
	ulqda/ulqda.pl \
	vpe/vpe.pl

# Symlinks within $(bindir): FILE:LINK indicates LINK->FILE
bin_links = \
	epstopdf:repstopdf \
	pdfcrop:rpdfcrop

# The idea is to install the scripts themselves in texmf*/scripts, and
# have bin/arch/foo be a symlink to, say,
# ../../texmf-dist/scripts/foo/foo.pl.  That way we save a bit of disk
# space, but much more importantly omit duplication, and most
# importantly of all make it possible to invoke the same Perl script on
# Windows (see ../w32_wrapper).
#
# The installation into texmf*/scripts is ideally done with ctan2tl.
# The copy of the wrapper for Windows is done by hand, but (todo)
# could/should also be done by ctan2tl.  The check-wrapper-consistency
# script at least checks after the fact for problems.
#
# We also keep a copy of all the scripts here and install into the runtime.
# This is purely for other distro builders, so symlinks are not dangling in
# the inst/bin dir (it is meaningless in native TL).  We have not yet
# written the auto-update to ensure linked_scripts is actually up to date
# with the masters in Master/texmf*/scripts, but one day.
#
# We support both multiplatform and non-multiplatform builds.
#
install-data-hook:
	case "$(bindir)" in \
	  */bin) $(MAKE) $(AM_MAKEFLAGS) REL=.. install-links;; \
	  */bin/*) $(MAKE) $(AM_MAKEFLAGS) REL=../.. install-links;; \
	  *) echo "strange directory '$(bindir)' for linked scripts" >&2; \
	     exit 1;; \
	esac

## Name the link as the basename, removing any extension,
## except for listings-ext.sh, which is documented as that name.
## Downcase for the sake of TeXcount.pl -> texcount.
.PHONY: install-links
install-links:
	@cd $(DESTDIR)$(bindir) && \
	  for s in $(nobase_dist_scriptx_SCRIPTS); do \
	    target=`basename $$s | sed 's,\.[^/]*$$,,' | tr '[A-Z]' '[a-z]'`; \
	    rm -f $$target; \
	    echo "creating link '$$target' -> '$(REL)/texmf/scripts/$$s'"; \
	    $(LN_S) $(REL)/texmf/scripts/$$s $$target; \
	  done && \
	  for s in $(nobase_dist_script_SCRIPTS); do \
	    target=`basename $$s | tr '[A-Z]' '[a-z]'`; \
	    echo "$$s" | grep listings-ext.sh >/dev/null \
	    || target=`echo $$target | sed 's,\.[^/]*$$,,'`; \
	    rm -f $$target; \
	    echo "creating link '$$target' -> '$(REL)/texmf-dist/scripts/$$s'"; \
	    $(LN_S) $(REL)/texmf-dist/scripts/$$s $$target; \
	  done && \
	  for s in $(bin_links); do \
	    link=`echo $$s | sed 's,.*:,,'`; \
	    file=`echo $$s | sed 's,:.*,,'`; \
	    rm -f $$link; \
	    echo "creating link '$$link' -> '$$file'"; \
	    $(LN_S) $$file $$link; \
	  done && \
	  rm -f man && \
	  if test -d $(REL)/texmf/doc/man; then \
	    echo "creating link 'man' -> '$(REL)/texmf/doc/man'" && \
	    (ln -s $(REL)/texmf/doc/man man || :); \
	  else :; fi
## man dir link for those mans which can use it.

uninstall-hook:
	@for s in $(nobase_dist_scriptx_SCRIPTS) $(nobase_dist_script_SCRIPTS); do \
	  target=`basename $$s | tr '[A-Z]' '[a-z]'`; \
	  echo "$$s" | grep listings-ext.sh >/dev/null \
	  || target=`echo $$target | sed 's,\.[^/]*$$,,'`; \
	  rm -f $(DESTDIR)$(bindir)/$$target; \
	done
	@for s in $(bin_links); do \
	  link=`echo $$s | sed 's,.*:,,'`; \
	  rm -f $(DESTDIR)$(bindir)/$$link; \
	done
	rm -f $(DESTDIR)$(bindir)/man

