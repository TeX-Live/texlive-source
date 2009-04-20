##
OS = UNIX
PAPER = a4
CHMOD = chmod
BINMODE = +x

AM_CPPFLAGS = -DUNIX -DPAPER=\"$(PAPER)\"

bin_PROGRAMS = epsffit psbook psnup psresize psselect pstops
bin_SCRIPTS = getafm showchar \
              extractres fixdlsrps fixfmps fixmacps fixpsditps fixpspps \
	      fixscribeps fixtpps fixwfwps fixwpps fixwwps includeres psmerge
CLEANFILES = $(bin_SCRIPTS)

PERL = /usr/bin/env perl
MAKETEXT = $(PERL) $(srcdir)/maketext

man1_MANS = epsffit.1 psbook.1 psnup.1 psresize.1 psselect.1 pstops.1 \
            getafm.1 \
	    extractres.1 fixdlsrps.man fixfmps.man fixmacps.man \
	    fixpsditps.man fixpspps.man fixscribeps.man fixtpps.man \
            fixwfwps.man fixwpps.man fixwwps.man includeres.man psmerge.man
dist_man_MANS = $(man1_MANS)

%: %.pl
	$(MAKETEXT) OS=$(OS) PERL="$(PERL)" $< >$@

%: %.sh
	cp $< $@
	$(CHMOD) $(BINMODE) $@

%.1: %.man
	$(MAKETEXT) PAPER=$(PAPER) "MAN=$(man1_MANS)" $< >$@

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

# scripts.
EXTRA_DIST += \
	getafm.sh \
	showchar.sh \
	extractres.pl \
	fixdlsrps.pl \
	fixfmps.pl \
	fixmacps.pl \
	fixpsditps.pl \
	fixpspps.pl \
	fixscribeps.pl \
	fixtpps.pl \
	fixwfwps.pl \
	fixwpps.pl \
	fixwwps.pl \
	includeres.pl \
	psmerge.pl

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
	maketext \
	md68_0.ps \
	md71_0.ps

## Eventually delete these files
##
EXTRA_DIST += Makefile.in.orig configure.in.orig
