## Makefile.am for the TeX Live subdirectory texk/dvipng/doc/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
#
## #################################### Documentation
##
info_TEXINFOS = dvipng.texi
dvipng_TEXINFOS = readme.texi install.texi macros.texi dvipng.help

dist_noinst_SCRIPTS = texi2pod.pl

dist_man1_MANS = dvipng.1
dvipng.1: dvipng.texi readme.texi
	$(srcdir)/texi2pod.pl -D man $(srcdir)/dvipng.texi | \
	sed -es/@//g -es/previewlatex/preview-latex/g -es/{}//g > dvipng.pod
	pod2man --center="User commands" --release="$(PACKAGE_STRING)" \
		dvipng.pod > dvipng.1
	rm dvipng.pod

www: dvipng.texi $(dvipng_TEXINFOS)
	$(mkdir_p) www
	$(TEXI2HTML) -split chapter -nosec-nav -subdir html \
	   -I $(srcdir) $(srcdir)/dvipng.texi
	(cd html; for i in *; do \
	   sed -e "s/Jan-A/Jan-\&Aring\;/g" $$i > ../www/$$i; \
	done) 
	cp www/dvipng.html www/index.html
	rm -rf html

dvipng_mono.html: dvipng.texi $(dvipng_TEXINFOS)
	$(TEXI2HTML) --monolithic -nomenu -nosec_nav -o dvipng_mono.html \
		 -I $(srcdir) $(srcdir)/dvipng.texi

distclean-local:
	rm -rf www
	rm -f dvipng_mono.html

if have_gif
install-data-hook:
	cd $(DESTDIR)$(man1dir) && \
	  rm -f dvigif.1 && \
	  $(LN_S) dvipng.1 dvigif.1
uninstall-hook:
	rm -f $(DESTDIR)$(man1dir)/dvigif.1
endif have_gif

## #################################### Maintainer targets
##
am-local: ../INSTALL ../README RELEASE

../INSTALL: install.texi
	-$(MAKEINFO) $(AM_MAKEINFOFLAGS) $(MAKEINFOFLAGS) -I $(srcdir) \
	  -D rawfile --no-headers --no-validate --no-number-sections \
	  -o $@ $<

../README: readme.texi
	-$(MAKEINFO) $(AM_MAKEINFOFLAGS) $(MAKEINFOFLAGS) -I $(srcdir) \
	  -D rawfile --no-headers --no-validate --no-number-sections \
	  -o $@ $<

