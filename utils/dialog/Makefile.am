## Proxy Makefile.am to build dialog for TeX Live.
##
##   Copyright (C) 2009, 2010 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
ACLOCAL_AMFLAGS = -I ../../m4

## The dialog-1.1-YYYYMMDD build system is good to build the binary,
## but lacks many required targets.
##
## DIALOG_TREE is the unmodified source tree.

# install dialog as tcdialog
program_transform_name = 's,^,tc,'

# We want to re-distribute the whole original dialog source tree.
EXTRA_DIST = $(DIALOG_TREE)

# Files not to distribute, also in case of an SVN repository
dist-hook:
	cd $(distdir)/$(DIALOG_TREE) && \
	  rm -rf config.log config.status dialog$(EXEEXT) dialog-config \
	    dlg_config.h headers-sh makefile samples/install/makefile \
	    *.a *.o  `find -name .svn`

nodist_man1_MANS = $(DIALOG_TREE)/dialog.1

bin_PROGRAMS = $(DIALOG_TREE)/dialog

$(DIALOG_TREE)/dialog$(EXEEXT): config.force
	cd $(DIALOG_TREE) && $(MAKE) $(AM_MAKEFLAGS) all

config.force:
	echo timestamp >config.force
	CONFIG_SHELL=$(SHELL) && export CONFIG_SHELL && \
	  eval $(SHELL) $(srcdir)/configure "`./config.status --config`"

distclean-local:
	if test -f "$(DIALOG_TREE)/Makefile"; then \
	  cd $(DIALOG_TREE) && $(MAKE) $(AM_MAKEFLAGS) distclean; \
	else :; fi

__DIALOG_TREE__dialog_SOURCES =

DISTCLEANFILES = config.force

