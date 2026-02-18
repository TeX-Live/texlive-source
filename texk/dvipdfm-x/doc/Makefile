# $Id$
# Minimal Makefile for dvipdfmx manual.
# Public domain. Originally written by Karl Berry, 2026.
# This is not run as part of the build, just a convenience used by hand
# from the Build/source/... directory. See tlpkg/doc/releng.txt.

texfot = texfot \
  --ignore 'Package microtype Warning: Unable to apply'

doc = dvipdfmx
$(doc).pdf: $(doc).tex fdl-1.3.tex
	$(texfot) xelatex $(doc).tex

install:

INSTALL_DATA = cp -pfv

texmf_dist = ../../../../../../Master/texmf-dist
dvipdfmx_doc_dir = $(texmf_dist)/doc/dvipdfmx
#
install: $(doc).pdf
	for f in Makefile $(doc).tex $(doc).pdf fdl-1.3.tex; do \
	  $(INSTALL_DATA) $$f $(dvipdfmx_doc_dir)/ || exit 1; done
	cd $(dvipdfmx_doc_dir) && /bin/pwd && svn status
