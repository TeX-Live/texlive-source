## Makefile.am for the TeX Live subdirectory texk/tex4htk/
##
## Copyright 2016 Karl Berry <tex-live@tug.org>
## Copyright 2009-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
## We distribute the whole java tree.
EXTRA_DIST = java

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am

AM_CPPFLAGS = $(KPATHSEA_INCLUDES) -DANSI -DKPATHSEA
AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = t4ht tex4ht

t4ht_SOURCES = t4ht.c

tex4ht_SOURCES = tex4ht.c

$(t4ht_OBJECTS) $(tex4ht_OBJECTS): $(KPATHSEA_DEPEND)

LDADD = $(KPATHSEA_LIBS)

## Rebuild libkpathsea
@KPATHSEA_RULE@

perl_scripts = mk4ht
shell_scripts = ht htcontext htlatex htmex httex httexi \
  htxelatex htxetex xhlatex

scriptsdir = texmf-dist/scripts/tex4ht
nodist_bin_SCRIPTS =

include $(srcdir)/../../am/script_links.am

texmfdir = $(datarootdir)/$(scriptsdir)
dist_texmf_SCRIPTS = $(perl_scripts:=.pl) $(shell_scripts:=.sh)

install-data-hook: install-perl-links install-shell-links
uninstall-hook: uninstall-links

if WIN32
dist_bin_SCRIPTS = $(shell_scripts:=.bat)
endif WIN32

CLEANFILES = $(nodist_bin_SCRIPTS)

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

