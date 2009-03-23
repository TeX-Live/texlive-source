##
OS = UNIX
PAPER = a4

AM_CPPFLAGS = -DUNIX -DPAPER=\"$(PAPER)\"

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

EXTRA_DIST = \
	LICENSE

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
	epsffit.man \
	extractres.man \
	extractres.pl \
	fixdlsrps.man \
	fixdlsrps.pl \
	fixfmps.man \
	fixfmps.pl \
	fixmacps.man \
	fixmacps.pl \
	fixpsditps.man \
	fixpsditps.pl \
	fixpspps.man \
	fixpspps.pl \
	fixscribeps.man \
	fixscribeps.pl \
	fixtpps.man \
	fixtpps.pl \
	fixwfwps.man \
	fixwfwps.pl \
	fixwpps.man \
	fixwpps.pl \
	fixwwps.man \
	fixwwps.pl \
	getafm.man \
	getafm.sh \
	includeres.man \
	includeres.pl \
	maketext \
	md68_0.ps \
	md71_0.ps \
	psbook.man \
	psnup.man \
	psmerge.man \
	psmerge.pl \
	psresize.man \
	psselect.man \
	pstops.man \
	showchar.sh

## Eventually delete these files
##
EXTRA_DIST += Makefile.in.orig configure.in.orig

