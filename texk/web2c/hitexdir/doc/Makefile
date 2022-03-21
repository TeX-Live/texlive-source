# $Id$
# Standalone Makefile for hitex/doc. Public domain.
# This is not run as part of the build, just a convenience used by hand
# from the Build/source/... directory.

INSTALL_DATA = cp -pfv

texmf_dist = ../../../../../../Master/texmf-dist
hitex_makeindex_dir = $(texmf_dist)/makeindex/hitex
hitex_doc_dir = $(texmf_dist)/doc/hitex/base
#
install:
	$(INSTALL_DATA) hint.ist $(hitex_makeindex_dir)
	for f in hiformat.hnt hiformat.pdf hintmac.tex \
	         hitexman.hnt hitexman.pdf hitexman.tex; do \
	  $(INSTALL_DATA) $$f $(hitex_doc_dir)/ || exit 1; done
	cd $(hitex_doc_dir) && /bin/pwd && svn status
