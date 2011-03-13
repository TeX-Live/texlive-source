## Makefile.am for the TeX Live subdirectory utils/xindy/
##
## Copyright (C) 2009 - 2011 Peter Breitenlohner <tex-live@tug.org>
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

bin_PROGRAMS =
if CLISP_RUNTIME
install-exec-hook:
	$(INSTALL_PROGRAM_ENV) $(INSTALL_PROGRAM) "$(LISP_RUN)" "$(DESTDIR)$(bindir)/$(XINDY_RUN)"
uninstall-hook:
	rm -f "$(DESTDIR)$(bindir)/$(XINDY_RUN)"
endif CLISP_RUNTIME

