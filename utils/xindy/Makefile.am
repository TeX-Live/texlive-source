## Makefile.am for the TeX Live subdirectory utils/xindy/
##
## Copyright (C) 2009-2012 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

EXTRA_DIST = README-clisp

## We use the original xindy source tree.
##
SUBDIRS = . $(XINDY_TREE)
DIST_SUBDIRS = $(SUBDIRS)

## Patches applied to the original source tree
##
EXTRA_DIST += $(XINDY_TREE)-PATCHES

## We must explicitly distribute some top-level files.
##
EXTRA_DIST += \
	$(XINDY_TREE)/AUTHORS \
	$(XINDY_TREE)/COPYING \
	$(XINDY_TREE)/INSTALL \
	$(XINDY_TREE)/Makefile.am \
	$(XINDY_TREE)/README \
	$(XINDY_TREE)/NEWS \
	$(XINDY_TREE)/VERSION \
	$(XINDY_TREE)/m4 \
	$(XINDY_TREE)/configure.ac

## For future reorganisation of alphabets/Makefile.am,
## used to build alphabets/LANG/Makefile.am fragments.
##
EXTRA_DIST += \
	$(XINDY_TREE)/make-rules/alphabets/Makefile.am.not-yet \
	$(XINDY_TREE)/make-rules/alphabets/lang.defs \
	$(XINDY_TREE)/make-rules/alphabets/mk-lang.sh

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`

all-local: config.force

config.force:
	echo timestamp >config.force
	$(SHELL) ./config.status --recheck

DISTCLEANFILES = config.force

perl_scripts = texindy xindy
scriptsdir = texmf/scripts/xindy
nodist_bin_SCRIPTS =

include $(srcdir)/../../texk/am/script_links.am

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
	sed -e 's:@is_TL\@:yes:' $< >$@
	chmod +x $@

CLEANFILES = $(nodist_bin_SCRIPTS) $(nodist_texmf_SCRIPTS)

