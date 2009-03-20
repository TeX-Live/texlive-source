#

dist_bin_SCRIPTS = \
	bg5latex \
	bg5pdflatex \
	cef5latex \
	cef5pdflatex \
	ceflatex \
	cefpdflatex \
	cefslatex \
	cefspdflatex \
	gbklatex \
	gbkpdflatex \
	sjislatex \
	sjispdflatex

install-exec-hook:
	@cd $(DESTDIR)$(bindir) && \
	  for p in latex pdflatex; do \
	    echo "$(LN_S) gbk$$p bg5+$$p"; \
	    rm -f bg5+$$p; $(LN_S) gbk$$p bg5+$$p; \
	  done

uninstall-hook:
	rm -f $(DESTDIR)$(bindir)/bg5+latex $(DESTDIR)$(bindir)/bg5+pdflatex

## Eventually delete these files
##
EXTRA_DIST = Makefile.in.orig

