# dist.mk -- making distribution tar files.
@MAINT@top_distdir = $(distname)-$(version)
@MAINT@top_files = RELNOTES Makefile.in configure.in configure selfautofix
@MAINT@libs_distdir = $(top_distdir)/libs
@MAINT@libs_files = EXTRA.LIB configure.in configure
@MAINT@texk_distdir = $(top_distdir)/texk
@MAINT@texk_files = ChangeLog Makefile.in README configure.in configure \
@MAINT@  install.sh libtool.m4 config.guess config.sub ltmain.sh \
@MAINT@  mkinstalldirs add-info-toc rename unbackslsh.awk withenable.ac
@MAINT@distdir = $(texk_distdir)/$(distname)
@MAINT@kpathsea_distdir = ../$(distname)/$(texk_distdir)/kpathsea
@MAINT@ln_files = AUTHORS ChangeLog INSTALL NEWS README *.in *.h *.c \
@MAINT@  configure *.mk
@MAINT@
@MAINT@dist_rm_predicate = -name Makefile
@MAINT@dist: all depend pre-dist-$(distname)
@MAINT@	rm -rf $(top_distdir)*
@MAINT@	mkdir -p $(distdir)
@MAINT@	top_files="$(top_files)"; \
@MAINT@	  for f in $$top_files; do \
@MAINT@	    ln $(top_srcdir)/../../$$f $(top_distdir); done
@MAINT@	cp -pr $(top_srcdir)/../../config $(top_distdir)
@MAINT@	mkdir -p $(libs_distdir)
@MAINT@	libs_files="$(libs_files)"; \
@MAINT@	  for f in $$libs_files; do \
@MAINT@	    ln $(top_srcdir)/../../libs/$$f $(libs_distdir); done
@MAINT@	cd .. && $(MAKE) Makefile ./configure
@MAINT@	cd .. && ln $(texk_files) $(distname)/$(texk_distdir)
@MAINT@	cp -p $(top_srcdir)/../dir $(texk_distdir)
@MAINT@	-ln $(ln_files) $(distdir)
@MAINT@	ln $(program_files) $(distdir)
@MAINT@	cd $(kpathsea_dir); $(MAKE) distdir=$(kpathsea_distdir) \
@MAINT@	  ln_files='$(ln_files)' distdir
@MAINT@	cp -pr ../make ../etc ../djgpp $(texk_distdir)
@MAINT@	rm -rf $(texk_distdir)/make/CVS
@MAINT@	rm -rf $(texk_distdir)/etc/CVS
@MAINT@	rm -rf $(texk_distdir)/etc/autoconf/CVS
@MAINT@	rm -rf $(texk_distdir)/djgpp/CVS
@MAINT@# Remove the extra files our patterns got us.
@MAINT@	cd $(texk_distdir); rm -f */c-auto.h
@MAINT@	find $(top_distdir) \( $(dist_rm_predicate) \) -exec rm '{}' \;
@MAINT@	find $(top_distdir) -name \.*texi -exec egrep -ni '	| ::|xx[^}]' \;
@MAINT@# Now handle the contrib dir.
@MAINT@	mkdir -p $(texk_distdir)/contrib && \
@MAINT@	  cp -p ../contrib/{ChangeLog,INSTALL,README,*.c,*.h} \
@MAINT@	        $(texk_distdir)/contrib
@MAINT@	$(MAKE) post-dist-$(distname)
@MAINT@	cd $(distdir); test ! -r *.info || touch *.info*
@MAINT@	chmod -R a+rX,u+w $(top_distdir)
@MAINT@	GZIP=-9 tar chzf $(top_distdir).tar.gz $(top_distdir)
@MAINT@	rm -rf $(top_distdir)
# End of dist.mk.
