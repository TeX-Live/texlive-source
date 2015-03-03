## Makefile.am for the TeX Live subdirectory texk/dvipng/doc/
##
## Copyright (C) 2009-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
#
info_TEXINFOS = \
	dvipng.texi
dvipng_TEXINFOS = \
	dvipng.help \
	install.texi \
	macros.texi \
	readme.texi

BUILT_SOURCES = doc-stamp

doc-stamp:
if MAINTAINER_MODE
## Check that the .texi files here are copies of those in ../$(DVIPNG_TREE)/.
	@for f in dvipng install macros readme; do \
	  f1="$(top_srcdir)/$(DVIPNG_TREE)/$$f.texi"; \
	  f2="$(srcdir)/$$f.texi"; \
	  echo "diff '$$f1' '$$f2'"; \
	  if diff "$$f1" "$$f2"; then :; \
	  else \
	    echo "cp -p '$$f1' '$$f2'"; \
	    cp -p "$$f1" "$$f2"; \
	  fi; \
	done
endif MAINTAINER_MODE
	echo timestamp >$@

dist_noinst_SCRIPTS = texi2pod.pl

dist_man1_MANS = dvipng.1
dvipng.1: dvipng.texi readme.texi
	$(srcdir)/texi2pod.pl -D man $(srcdir)/dvipng.texi | \
	sed -es/@//g -es/previewlatex/preview-latex/g -es/{}//g > dvipng.pod
	pod2man --center="User commands" --release="$(PACKAGE_STRING)" \
		dvipng.pod > dvipng.1
	rm dvipng.pod

# Symlinks within $(man1dir): FILE:LINK indicates LINK.1->FILE.1
man1_links = dvipng:dvigif

include $(top_srcdir)/../../am/man1_links.am

if have_gif
install-data-hook: install-man1-links
uninstall-hook: uninstall-man1-links
endif have_gif

DISTCLEANFILES = doc-stamp

