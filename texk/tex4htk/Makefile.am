## Makefile.am for the TeX Live subdirectory texk/tex4htk/
##
## Copyright (C) 2009-2010 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

INCLUDES = $(KPATHSEA_INCLUDES)
AM_CPPFLAGS = -DANSI -DKPATHSEA
AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = t4ht tex4ht

t4ht_SOURCES = t4ht.c

tex4ht_SOURCES = tex4ht.c

$(t4ht_OBJECTS) $(tex4ht_OBJECTS): $(KPATHSEA_DEPEND)

LDADD = $(KPATHSEA_LIBS)

## Rebuild libkpathsea
@KPATHSEA_RULE@

perl_scripts = mk4ht
shell_scripts = ht htcontext htlatex htmex httex httexi htxelatex htxetex

EXTRA_DIST = $(perl_scripts) $(shell_scripts)

## Make sure $(tex4htdir) exists
##
tex4ht_subdir = texmf-dist/scripts/tex4ht
tex4htdir = ${prefix}/$(tex4ht_subdir)
tex4ht_SCRIPTS =

# We support both multiplatform and non-multiplatform builds.
install-data-hook:
	@for p in $(perl_scripts); do \
	  echo "$(INSTALL_SCRIPT) $(srcdir)/$$p '$(DESTDIR)$(tex4htdir)/$$p.pl'"; \
	  $(INSTALL_SCRIPT) $(srcdir)/$$p "$(DESTDIR)$(tex4htdir)/$$p.pl"; \
	done
	@for p in $(shell_scripts); do \
	  echo "$(INSTALL_SCRIPT) $(srcdir)/$$p '$(DESTDIR)$(tex4htdir)/$$p.sh'"; \
	  $(INSTALL_SCRIPT) $(srcdir)/$$p "$(DESTDIR)$(tex4htdir)/$$p.sh"; \
	done
	case "$(bindir)" in \
	  */bin) $(MAKE) $(AM_MAKEFLAGS) REL=.. install-links;; \
	  */bin/*) $(MAKE) $(AM_MAKEFLAGS) REL=../.. install-links;; \
	  *) echo "strange directory '$(bindir)' for linked scripts" >&2; \
	     exit 1;; \
	esac

## Make sure $(bindir) exists
##
bin_SCRIPTS =

## Link to the basename, removing the extension,
.PHONY: install-links
install-links:
	@cd $(DESTDIR)$(bindir) && \
	  for p in $(perl_scripts); do \
	    rm -f $$p; \
	    echo "creating link '$$p' -> '$(REL)/$(tex4ht_subdir)/$$p.pl'"; \
	    $(LN_S) $(REL)/$(tex4ht_subdir)/$$p.pl $$p; \
	  done && \
	  for p in $(shell_scripts); do \
	    rm -f $$p; \
	    echo "creating link '$$p' -> '$(REL)/$(tex4ht_subdir)/$$p.sh'"; \
	    $(LN_S) $(REL)/$(tex4ht_subdir)/$$p.sh $$p; \
	  done

uninstall-hook:
	@for p in $(perl_scripts:=.pl) $(shell_scripts:=.sh); do \
	  echo "rm -f '$(DESTDIR)$(tex4htdir)/$$p'"; \
	  rm -f "$(DESTDIR)$(tex4htdir)/$$p"; \
	done
	@for p in $(perl_scripts) $(shell_scripts); do \
	  echo "rm -f '$(DESTDIR)$(bindir)/$$p'"; \
	  rm -f "$(DESTDIR)$(bindir)/$$p"; \
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

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`

