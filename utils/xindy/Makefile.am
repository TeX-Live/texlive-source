## $Id$
## Makefile.am for the TeX Live subdirectory utils/xindy/
##
## Copyright 2017 Karl Berry <tex-live@tug.org>
## Copyright 2009-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.

EXTRA_DIST = README-clisp

## We use the original xindy source tree.
SUBDIRS = . $(XINDY_TREE)
DIST_SUBDIRS = $(SUBDIRS)

## Patches applied to the original source tree
EXTRA_DIST += TLpatches

## We must explicitly distribute some top-level files.
##
EXTRA_DIST += \
	$(XINDY_TREE)/Makefile.am \
	$(XINDY_TREE)/VERSION \
	$(XINDY_TREE)/configure.ac \
	$(XINDY_TREE)/m4

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am

all-local: config.force

config.force:
	echo timestamp >config.force
	$(SHELL) ./config.status --recheck

DISTCLEANFILES = config.force

perl_scripts = texindy xindy
scriptsdir = texmf-dist/scripts/xindy
nodist_bin_SCRIPTS =

include $(srcdir)/../../am/script_links.am

install-data-hook: install-perl-links

install-exec-hook:
if CLISP_RUNTIME
	$(INSTALL_PROGRAM_ENV) $(INSTALL_PROGRAM) "$(LISP_RUN)" "$(DESTDIR)$(bindir)/$(XINDY_RUN)"
endif CLISP_RUNTIME

uninstall-hook: uninstall-links
if CLISP_RUNTIME
	rm -f "$(DESTDIR)$(bindir)/$(XINDY_RUN)"
endif CLISP_RUNTIME

texmfdir = $(datarootdir)/$(scriptsdir)
nodist_texmf_SCRIPTS = $(perl_scripts:=.pl)

%.pl: $(XINDY_TREE)/user-commands/%.in
	$(AM_V_GEN)sed -e 's:@is_TL@:yes:' $< >$@
	$(AM_V_at)chmod +x $@

CLEANFILES = $(nodist_bin_SCRIPTS) $(nodist_texmf_SCRIPTS)

