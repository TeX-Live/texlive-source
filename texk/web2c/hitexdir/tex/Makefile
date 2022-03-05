# $Id$
# Standalone Makefile for hitex/tex. Public domain.
# This is not run as part of the build, just a convenience used by hand
# from the Build/source/... directory.

INSTALL_DATA = cp -pfv

texmf_dist = ../../../../../../Master/texmf-dist
hitex_doc_dir = $(texmf_dist)/tex/hitex/base
#
install:
	for f in hilatex.ini hiltxpage.tex hiplainpage.tex \
	         hitex.ini ifhint.tex; do \
	  $(INSTALL_DATA) $$f $(hitex_doc_dir)/ || exit 1; done

fmt: hitex.fmt
hitex.fmt: hitex.ini
	hitex -ini -etex -ltx $<

clean:
	rm -f *.fmt *.log *.aux *.hint *.hnt
