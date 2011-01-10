## Makefile.am for the TeX Live subdirectory utils/ps2eps/
##
## Copyright (C) 2009 - 2011 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

## We want to re-distribute the whole original ps2eps source tree.
EXTRA_DIST = $(PS2EPS_TREE)
## Patches applied to the original source tree
EXTRA_DIST += $(PS2EPS_TREE)-PATCHES

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`

AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = bbox

bbox_SOURCES = \
	$(PS2EPS_TREE)/src/C/bbox.c

man1_MANS = \
	$(PS2EPS_TREE)/doc/man/man1/bbox.1 \
	$(PS2EPS_TREE)/doc/man/man1/ps2eps.1

perl_scripts = ps2eps

scriptsdir = texmf/scripts/ps2eps
texmfdir = ${prefix}/$(scriptsdir)
nodist_texmf_SCRIPTS = $(PS2EPS_TREE)/bin/ps2eps.pl

if WIN32
if WIN32_WRAP
## We treat the WIN32 wrappers as SCRIPTS to avoid automatic build rules
nodist_bin_SCRIPTS = $(perl_scripts:=.exe)
runscript = $(srcdir)/../../texk/texlive/w32_wrapper/runscript.exe
$(nodist_bin_SCRIPTS): $(runscript)
	$(LN_S) $(runscript) $@
endif WIN32_WRAP
endif WIN32

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

CLEANFILES = $(nodist_bin_SCRIPTS)

