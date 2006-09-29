pkgdatadir = $(datadir)/texmf/dvipdfm
pkgdata_DATA = CMap config
EXTRA_DIST   = $(pkgdata_DATA)

install-pkgdataDATA: $(pkgdata_DATA)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)
	@list='$(pkgdata_DATA)'; for adir in $$list; do \
	  $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/$$adir; \
	  for p in $$adir/*; do \
	    if test -f $(srcdir)/$$p; then \
	      echo " $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(pkgdatadir)/$$p"; \
	      $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(pkgdatadir)/$$p; \
	    else if test -f $$p; then \
	      echo " $(INSTALL_DATA) $$p $(DESTDIR)$(pkgdatadir)/$$p"; \
	      $(INSTALL_DATA) $$p $(DESTDIR)$(pkgdatadir)/$$p; \
	    fi; fi; \
	  done; \
	done

uninstall-pkgdataDATA:
	@$(NORMAL_UNINSTALL)
	list='$(pkgdata_DATA)'; for p in $$list; do \
	  rm -rf $(DESTDIR)$(pkgdatadir)/$$p; \
	done
