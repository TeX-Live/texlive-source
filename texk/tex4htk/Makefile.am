##
ACLOCAL_AMFLAGS = -I ../../m4

INCLUDES = $(KPATHSEA_INCLUDES)
AM_CPPFLAGS = -DUNIX -DKPATHSEA -DNeedFunctionPrototypes

bin_PROGRAMS = t4ht tex4ht

t4ht_SOURCES = t4ht.c

t4ht_DEPENDENCIES = $(KPATHSEA_DEPEND)

tex4ht_SOURCES = tex4ht.c

tex4ht_DEPENDENCIES = $(KPATHSEA_DEPEND)

LDADD = $(KPATHSEA_LIBS)

## Rebuild libkpathsea
@KPATHSEA_RULE@

perl_scripts = mk4ht
shell_scripts = ht htcontext htlatex htmex httex httexi htxelatex htxetex

EXTRA_DIST = $(perl_scripts) $(shell_scripts)

install-exec-hook:
	$(mkdir_p) "$(DESTDIR)$(tex4htdir)" "$(DESTDIR)$(bindir)"
	@for p in $(perl_scripts); do \
	  echo "$(INSTALL_SCRIPT) $(srcdir)/$$p '$(DESTDIR)$(tex4htdir)/$$p.pl'"; \
	  $(INSTALL_SCRIPT) $(srcdir)/$$p "$(DESTDIR)$(tex4htdir)/$$p.pl"; \
	done
	@for p in $(shell_scripts); do \
	  echo "$(INSTALL_SCRIPT) $(srcdir)/$$p '$(DESTDIR)$(tex4htdir)/$$p.sh'"; \
	  $(INSTALL_SCRIPT) $(srcdir)/$$p "$(DESTDIR)$(tex4htdir)/$$p.sh"; \
	done
	@cd $(DESTDIR)$(bindir) && \
	  for p in $(perl_scripts); do \
	    echo "$(LN_S) $(tex4htrel)/$$p.pl $$p"; \
	    rm -f $$p; $(LN_S) $(tex4htrel)/$$p.pl $$p; \
	  done; \
	  for p in $(shell_scripts); do \
	    echo "$(LN_S) $(tex4htrel)/$$p.sh $$p"; \
	    rm -f $$p; $(LN_S) $(tex4htrel)/$$p.sh $$p; \
	  done

uninstall-hook:
	@for p in $(perl_scripts) $(shell_scripts); do \
	  echo "rm -f '$(DESTDIR)$(bindir)/$$p'"; \
	  rm -f "$(DESTDIR)$(bindir)/$$p"; \
	done
	@for p in $(perl_scripts:=.pl) $(shell_scripts:=.sh); do \
	  echo "rm -f '$(DESTDIR)$(tex4htdir)/$$p'"; \
	  rm -f "$(DESTDIR)$(tex4htdir)/$$p"; \
	done

## Not used
##
EXTRA_DIST += \
	dbcontext \
	dblatex \
	dbmcontext \
	dbmex \
	dbmlatex \
	dbmmex \
	dbmtex \
	dbmtexi \
	dbmxelatex \
	dbmxetex \
	dbtex \
	dbtexi \
	dbxelatex \
	dbxetex \
	demo.tex \
	escontext \
	eslatex \
	esmex \
	estex \
	estexi \
	esxelatex \
	esxetex \
	htcmd.c \
	java \
	jh1context \
	jh1latex \
	jh1mex \
	jh1tex \
	jh1texi \
	jh1xelatex \
	jh1xetex \
	jhcontext \
	jhlatex \
	jhmex \
	jhtex \
	jhtexi \
	jhxelatex \
	jhxetex \
	jmcontext \
	jmlatex \
	jmmex \
	jmtex \
	jmtexi \
	jmxelatex \
	jmxetex \
	jscontext \
	jslatex \
	jsmex \
	jstex \
	jstexi \
	jsxelatex \
	jsxetex \
	mzcontext \
	mzlatex \
	mzmex \
	mztex \
	mztexi \
	mzxelatex \
	mzxetex \
	oocontext \
	oolatex \
	oomex \
	ootex \
	ootexi \
	ooxelatex \
	ooxetex \
	teicontext \
	teilatex \
	teimcontext \
	teimex \
	teimlatex \
	teimmex \
	teimtex \
	teimtexi \
	teimxelatex \
	teimxetex \
	teitex \
	teitexi \
	teixelatex \
	teixetex \
	test.tex \
	test1.tex \
	testa.tex \
	testb.tex \
	uxhcontext \
	uxhlatex \
	uxhmex \
	uxhtex \
	uxhtexi \
	uxhxelatex \
	uxhxetex \
	wcontext \
	wlatex \
	wmex \
	wtex \
	wtexi \
	wxelatex \
	wxetex \
	xhcontext \
	xhlatex \
	xhmcontext \
	xhmex \
	xhmlatex \
	xhmmex \
	xhmtex \
	xhmtexi \
	xhmxelatex \
	xhmxetex \
	xhtex \
	xhtexi \
	xhxelatex \
	xhxetex \
	xv4ht.java

## in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`

## Eventually delete these files
##
EXTRA_DIST += Makefile.in.orig configure.in.orig

