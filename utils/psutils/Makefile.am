## Makefile.am for the TeX Live subdirectory utils/psutils/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

OS = UNIX
PAPER = a4

AM_CPPFLAGS = -DUNIX -DPAPER=\"$(PAPER)\"
AM_CFLAGS = $(WARNING_CFLAGS)

bin_PROGRAMS = epsffit psbook psnup psresize psselect pstops

epsffit_SOURCES = \
	config.h \
	epsffit.c \
	pserror.c \
	pserror.h

psbook_SOURCES = \
	config.h \
	psbook.c \
	pserror.c \
	pserror.h \
	psutil.c \
	psutil.h

psnup_SOURCES = \
	config.h \
	pserror.c \
	pserror.h \
	psnup.c \
	psspec.c \
	psspec.h \
	psutil.c \
	psutil.h

psresize_SOURCES = \
	config.h \
	pserror.c \
	pserror.h \
	psresize.c \
	psspec.c \
	psspec.h \
	psutil.c \
	psutil.h

psselect_SOURCES = \
	config.h \
	pserror.c \
	pserror.h \
	psselect.c \
	psutil.c \
	psutil.h

pstops_SOURCES = \
	pserror.c \
	pserror.h \
	psspec.c \
	psspec.h \
	pstops.c \
	psutil.c \
	psutil.h

## 
EXTRA_DIST = \
	LICENSE

perl_scripts = extractres fixdlsrps fixfmps fixmacps fixpsditps fixpspps \
	fixscribeps fixtpps fixwfwps fixwpps fixwwps includeres psmerge

shell_scripts = getafm showchar

nodist_bin_SCRIPTS = $(perl_scripts)

install-exec-hook:
	@for f in $(shell_scripts); do \
	  echo "$(INSTALL_SCRIPT) '$(srcdir)/$$f.sh' '$(DESTDIR)$(bindir)/$$f'"; \
	  $(INSTALL_SCRIPT) "$(srcdir)/$$f.sh" "$(DESTDIR)$(bindir)/$$f"; \
	done

uninstall-hook:
	@for f in $(shell_scripts); do \
	  echo "rm -f '$(DESTDIR)$(bindir)/$$f'"; \
	  rm -f "$(DESTDIR)$(bindir)/$$f"; \
	done

dist_man1_MANS = \
	fixdlsrps.man \
	fixfmps.man \
	fixmacps.man \
	fixpsditps.man \
	fixpspps.man \
	fixscribeps.man \
	fixtpps.man \
	fixwfwps.man \
	fixwpps.man \
	fixwwps.man \
	getafm.man \
	includeres.man \
	psmerge.man

nodist_man1_MANS = \
	epsffit.1\
	extractres.1 \
	psbook.1 \
	psnup.1 \
	psresize.1 \
	psselect.1 \
	pstops.1

EXTRA_DIST += $(perl_scripts:=.pl) $(shell_scripts:=.sh) $(nodist_man1_MANS:.1=.man)

CLEANFILES = $(nodist_bin_SCRIPTS) $(nodist_man1_MANS)

PERL = /usr/bin/env perl
MAKETEXT = $(PERL) $(srcdir)/maketext

EXTRA_DIST += maketext

%: %.pl
	$(MAKETEXT) OS=$(OS) PERL="$(PERL)" $< >$@

%.1: %.man
	$(MAKETEXT) PAPER=$(PAPER) "MAN=$(man1_MANS)" $< >$@

## Not used
##
EXTRA_DIST += \
	Makefile.bcc \
	Makefile.msc \
	Makefile.nt \
	Makefile.os2 \
	Makefile.sc \
	Makefile.unix \
	Makefile.wat \
	descrip.mms \
	md68_0.ps \
	md71_0.ps

