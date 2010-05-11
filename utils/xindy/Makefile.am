## Makefile.am for the TeX Live subdirectory texk/dvipng/
##
## Copyright (C) 2009, 2010 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

EXTRA_DIST = README-clisp

## We use the original xindy source tree.
##
SUBDIRS = . xindy

## Patches applied to the original source tree
##
EXTRA_DIST += xindy-$(VERSION)-PATCHES

## We must explicitly distribute some top-level files.
##
EXTRA_DIST += \
	xindy/AUTHORS \
	xindy/COPYING \
	xindy/INSTALL \
	xindy/Makefile.am \
	xindy/README \
	xindy/NEWS \
	xindy/m4 \
	xindy/configure.ac

## For future reorganisation of alphabets/Makefile.am,
## used to build alphabets/LANG/Makefile.am fragments.
##
EXTRA_DIST += \
	xindy/make-rules/alphabets/Makefile.am.not-yet \
	xindy/make-rules/alphabets/lang.defs \
	xindy/make-rules/alphabets/mk-lang.sh

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

