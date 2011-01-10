## Makefile.am for the TeX Live subdirectory utils/chktex/
##
## Copyright (C) 2010, 2011 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
#************************************************************************
#
#  Adapted for TeX Live from chktex/Makefile.1n
#
#************************************************************************
##
ACLOCAL_AMFLAGS = -I ../../m4

## We want to re-distribute the whole original chktex source tree.
##
## With current automake (1.10.2) 'make distcheck' fails when
## DISTFILES contains a directory and files in that directory.
## Thus nodist_* for all files in $(CHKTEX_TREE).
EXTRA_DIST = $(CHKTEX_TREE)

## Patches applied to the original source tree
##
EXTRA_DIST += $(CHKTEX_TREE)-PATCHES

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`

AM_CPPFLAGS = -DNO_KPSE_DLL
AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = chktex

chktex_SOURCES = \
	getopt.c \
	getopt.h \
	getopt1.c \
	$(CHKTEX_TREE)/ChkTeX.c \
	$(CHKTEX_TREE)/ChkTeX.h \
	$(CHKTEX_TREE)/FindErrs.c \
	$(CHKTEX_TREE)/FindErrs.h \
	$(CHKTEX_TREE)/OpSys.c \
	$(CHKTEX_TREE)/OpSys.h \
	$(CHKTEX_TREE)/Resource.c \
	$(CHKTEX_TREE)/Resource.h \
	$(CHKTEX_TREE)/Utility.c \
	$(CHKTEX_TREE)/Utility.h \
	$(CHKTEX_TREE)/types.h

man_MANS = \
	$(CHKTEX_TREE)/chktex.1 \
	$(CHKTEX_TREE)/chkweb.1 \
	$(CHKTEX_TREE)/deweb.1

perl_scripts = deweb

scriptsdir = texmf/scripts/chktex
texmfdir = ${prefix}/$(scriptsdir)
nodist_texmf_SCRIPTS = $(perl_scripts:=.pl)

if WIN32
if WIN32_WRAP
## We treat the WIN32 wrappers as SCRIPTS to avoid automatic build rules
nodist_bin_SCRIPTS = $(perl_scripts:=.exe)
runscript = $(srcdir)/../../texk/texlive/w32_wrapper/runscript.exe
$(nodist_bin_SCRIPTS): $(runscript)
	$(LN_S) $(runscript) $@
endif WIN32_WRAP
else !WIN32
bin_SCRIPTS = $(CHKTEX_TREE)/chkweb
endif !WIN32

install-data-hook:
if !WIN32
	case "$(bindir)" in \
	  */bin) $(MAKE) $(AM_MAKEFLAGS) REL=.. install-links;; \
	  */bin/*) $(MAKE) $(AM_MAKEFLAGS) REL=../.. install-links;; \
	  *) echo "strange directory '$(bindir)' for linked scripts" >&2; \
	     exit 1;; \
	esac
endif !WIN32

.PHONY: install-links
install-links:
	@cd $(DESTDIR)$(bindir) && \
	  for f in $(perl_scripts); do \
	    rm -f $$f; \
	    echo "creating link '$$f' -> '$(REL)/$(scriptsdir)/$$f.pl'"; \
	    $(LN_S) $(REL)/$(scriptsdir)/$$f.pl $$f || exit 1; \
	  done  

uninstall-hook:
if !WIN32
	@for f in $(perl_scripts); do \
	  echo "rm -f '$(DESTDIR)$(bindir)/$$f'"; \
	  rm -f "$(DESTDIR)$(bindir)/$$f"; \
	done
endif !WIN32

# resource file
configdir = ${prefix}/texmf/chktex

config_DATA = $(CHKTEX_TREE)/chktexrc

# documentation
pdfdocdir = ${prefix}/texmf/doc/chktex

dist_pdfdoc_DATA = ChkTeX.pdf

ChkTeX.pdf: $(PDF_DEPEND)
	$(PDFLATEX) -interaction batchmode $(srcdir)/ChkTeX.tex
	$(PDFLATEX) -interaction batchmode $(srcdir)/ChkTeX.tex

CLEANFILES = $(nodist_bin_SCRIPTS) ChkTeX.aux ChkTeX.log

## We distribute ChkTeX.tex (generated from $(CHKTEX_TREE)/ChkTeX.tex.in)
## and ChkTeX.pdf, but also need rules to create them if missing or
## (in maintainer-mode) to update them if required.
.PHONY: build-tex
all-local: stamp-tex
stamp-tex: ChkTeX.tmp
	$(MAKE) $(AM_MAKEFLAGS) build-tex
	$(MAKE) $(AM_MAKEFLAGS) ChkTeX.pdf
	echo timestamp >$@
DISTCLEANFILES = stamp-tex
build-tex:
	cmp -s ChkTeX.tmp $(srcdir)/ChkTeX.tex \
	  || cp ChkTeX.tmp $(srcdir)/ChkTeX.tex

EXTRA_DIST += ChkTeX.tex

# chktex tests
TESTS = chktex.test

TESTS_ENVIRONMENT = CHKTEX_TREE=$(CHKTEX_TREE)

EXTRA_DIST += chktex.test

CLEANFILES += chktest

