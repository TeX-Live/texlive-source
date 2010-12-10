# Makefile for PS utilities release 1 patchlevel 17
#
# Copyright (C) Angus J. C. Duggan 1991-1996
# See file LICENSE for details.
#
# updated AJCD 3/1/96
#
# Multiple makefiles for different OSes are generated from a single master
# now.
#
# There are so many incompatible makes around that putting all of the
# non-standard targets explicitly seems to be the only way of ensuring
# portability.

# epsffit fits an epsf file to a given bounding box
# psbook rearranges pages into signatures
# psselect selects page ranges
# pstops performs general page rearrangement and merging
# psnup puts multiple logical pages on one physical page
# psresize scales and moves pages to fit on different paper sizes

PAPER=a4

# Makefile for PSUtils under Unix

OS = UNIX

BINDIR = /usr/local/bin
SCRIPTDIR = $(BINDIR)
INCLUDEDIR = /usr/local/share/psutils
PERL = /usr/local/bin/perl

BINMODE = 0755
MANMODE = 0644
CHMOD = chmod
INSTALL = install -c -m $(BINMODE)
INSTALLMAN = install -c -m $(MANMODE)
MANEXT = 1
MANDIR = /usr/local/share/man/man$(MANEXT)

CC = gcc
CFLAGS = -DPAPER=\"$(PAPER)\" -DUNIX -O -Wall

BIN = psbook psselect pstops epsffit psnup \
	psresize
SHELLSCRIPTS = getafm showchar
PERLSCRIPTS = fixfmps fixmacps fixpsditps fixpspps \
	fixtpps fixwfwps fixwpps fixscribeps fixwwps \
	fixdlsrps extractres includeres psmerge
MANPAGES = psbook.$(MANEXT) psselect.$(MANEXT) pstops.$(MANEXT) epsffit.$(MANEXT) psnup.$(MANEXT) \
	psresize.$(MANEXT) psmerge.$(MANEXT) fixscribeps.$(MANEXT) getafm.$(MANEXT) \
	fixdlsrps.$(MANEXT) fixfmps.$(MANEXT) fixmacps.$(MANEXT) fixpsditps.$(MANEXT) \
	fixpspps.$(MANEXT) fixtpps.$(MANEXT) fixwfwps.$(MANEXT) fixwpps.$(MANEXT) \
	fixwwps.$(MANEXT) extractres.$(MANEXT) includeres.$(MANEXT)
INCLUDES = md68_0.ps md71_0.ps

all: $(BIN) $(PERLSCRIPTS) $(MANPAGES) $(SHELLSCRIPTS)

psutil.o: psutil.h patchlev.h pserror.h psutil.c

psspec.o: psutil.h patchlev.h psspec.h pserror.h psspec.c

pserror.o: psutil.h patchlev.h pserror.h pserror.c

epsffit.o: epsffit.c pserror.h patchlev.h

epsffit: epsffit.o pserror.o
	$(CC) $(CCFLAGS) -o epsffit pserror.o epsffit.o

psnup: psnup.o psutil.o psspec.o pserror.o
	$(CC) $(CCFLAGS) -o psnup psutil.o psspec.o pserror.o psnup.o

psnup.o: psutil.h patchlev.h psspec.h pserror.h psnup.c

psresize: psresize.o psutil.o pserror.o psspec.o
	$(CC) $(CCFLAGS) -o psresize psutil.o psspec.o pserror.o psresize.o

psresize.o: psutil.h patchlev.h psspec.h pserror.h psresize.c

psbook: psbook.o psutil.o pserror.o
	$(CC) $(CCFLAGS) -o psbook psutil.o pserror.o psbook.o

psbook.o: psutil.h patchlev.h pserror.h psbook.c

psselect: psselect.o psutil.o pserror.o
	$(CC) $(CCFLAGS) -o psselect psutil.o pserror.o psselect.o

psselect.o: psutil.h patchlev.h pserror.h psselect.c

pstops: pstops.o psutil.o psspec.o pserror.o
	$(CC) $(CCFLAGS) -o pstops psutil.o psspec.o pserror.o pstops.o

pstops.o: psutil.h patchlev.h psspec.h pserror.h pstops.c

getafm:	getafm.sh
	cp $? $@

showchar:	showchar.sh
	cp $? $@

psmerge: psmerge.pl
	$(PERL) maketext OS=$(OS) PERL=$(PERL) $? > $@
	$(CHMOD) $(BINMODE) $@

fixfmps: fixfmps.pl
	$(PERL) maketext OS=$(OS) PERL=$(PERL) $? > $@
	$(CHMOD) $(BINMODE) $@

fixmacps: fixmacps.pl
	$(PERL) maketext OS=$(OS) PERL=$(PERL) INCLUDE=$(INCLUDEDIR) $? > $@
	$(CHMOD) $(BINMODE) $@

fixpsditps: fixpsditps.pl
	$(PERL) maketext OS=$(OS) PERL=$(PERL) $? > $@
	$(CHMOD) $(BINMODE) $@

fixpspps: fixpspps.pl
	$(PERL) maketext OS=$(OS) PERL=$(PERL) $? > $@
	$(CHMOD) $(BINMODE) $@

fixscribeps: fixscribeps.pl
	$(PERL) maketext OS=$(OS) PERL=$(PERL) $? > $@
	$(CHMOD) $(BINMODE) $@

fixtpps: fixtpps.pl
	$(PERL) maketext OS=$(OS) PERL=$(PERL) $? > $@
	$(CHMOD) $(BINMODE) $@

fixwfwps: fixwfwps.pl
	$(PERL) maketext OS=$(OS) PERL=$(PERL) $? > $@
	$(CHMOD) $(BINMODE) $@

fixwpps: fixwpps.pl
	$(PERL) maketext OS=$(OS) PERL=$(PERL) $? > $@
	$(CHMOD) $(BINMODE) $@

fixwwps: fixwwps.pl
	$(PERL) maketext OS=$(OS) PERL=$(PERL) $? > $@
	$(CHMOD) $(BINMODE) $@

fixdlsrps: fixdlsrps.pl
	$(PERL) maketext OS=$(OS) PERL=$(PERL) $? > $@
	$(CHMOD) $(BINMODE) $@

extractres: extractres.pl
	$(PERL) maketext OS=$(OS) PERL=$(PERL) $? > $@
	$(CHMOD) $(BINMODE) $@

includeres: includeres.pl
	$(PERL) maketext OS=$(OS) PERL=$(PERL) INCLUDE=$(INCLUDEDIR) $? > $@
	$(CHMOD) $(BINMODE) $@

epsffit.$(MANEXT): epsffit.man
	$(PERL) maketext MAN="$(MANPAGES)" $? > $@

psnup.$(MANEXT): psnup.man
	$(PERL) maketext MAN="$(MANPAGES)" PAPER=$(PAPER) $? > $@

psresize.$(MANEXT): psresize.man
	$(PERL) maketext MAN="$(MANPAGES)" PAPER=$(PAPER) $? > $@

psbook.$(MANEXT): psbook.man
	$(PERL) maketext "MAN=$(MANPAGES)" $? > $@

psselect.$(MANEXT): psselect.man
	$(PERL) maketext "MAN=$(MANPAGES)" $? > $@

pstops.$(MANEXT): pstops.man
	$(PERL) maketext "MAN=$(MANPAGES)" PAPER=$(PAPER) $? > $@

psmerge.$(MANEXT): psmerge.man
	$(PERL) maketext "MAN=$(MANPAGES)" $? > $@

fixfmps.$(MANEXT): fixfmps.man
	$(PERL) maketext "MAN=$(MANPAGES)" $? > $@

fixmacps.$(MANEXT): fixmacps.man
	$(PERL) maketext "MAN=$(MANPAGES)" INCLUDE=$(INCLUDEDIR) $? > $@

fixpsditps.$(MANEXT): fixpsditps.man
	$(PERL) maketext "MAN=$(MANPAGES)" $? > $@

fixpspps.$(MANEXT): fixpspps.man
	$(PERL) maketext "MAN=$(MANPAGES)" $? > $@

fixscribeps.$(MANEXT): fixscribeps.man
	$(PERL) maketext "MAN=$(MANPAGES)" $? > $@

fixtpps.$(MANEXT): fixtpps.man
	$(PERL) maketext "MAN=$(MANPAGES)" $? > $@

fixwfwps.$(MANEXT): fixwfwps.man
	$(PERL) maketext "MAN=$(MANPAGES)" $? > $@

fixwpps.$(MANEXT): fixwpps.man
	$(PERL) maketext "MAN=$(MANPAGES)" $? > $@

fixwwps.$(MANEXT): fixwwps.man
	$(PERL) maketext "MAN=$(MANPAGES)" $? > $@

fixdlsrps.$(MANEXT): fixdlsrps.man
	$(PERL) maketext "MAN=$(MANPAGES)" $? > $@

extractres.$(MANEXT): extractres.man
	$(PERL) maketext "MAN=$(MANPAGES)" $? > $@

includeres.$(MANEXT): includeres.man
	$(PERL) maketext "MAN=$(MANPAGES)" INCLUDE=$(INCLUDEDIR) $? > $@

getafm.$(MANEXT): getafm.man
	$(PERL) maketext "MAN=$(MANPAGES)" $? > $@

clean:
	rm -f *.o

veryclean realclean: clean
	rm -f $(BIN) $(PERLSCRIPTS) $(MANPAGES)

install: install.bin install.script install.man install.include

install.bin: $(BIN)
	-mkdir $(BINDIR)
	@for i in $(BIN); do \
		echo Installing $$i; \
		$(INSTALL) $$i $(BINDIR); \
	done

install.script: $(PERLSCRIPTS) $(SHELLSCRIPTS)
	-mkdir $(SCRIPTDIR)
	@for i in $(PERLSCRIPTS) $(SHELLSCRIPTS); do \
		echo Installing $$i; \
		$(INSTALL) $$i $(SCRIPTDIR); \
	done

install.include: $(INCLUDES)
	-mkdir $(INCLUDEDIR)
	@for i in $(INCLUDES); do \
		echo Installing $$i; \
		$(INSTALLMAN) $$i $(INCLUDEDIR); \
	done

install.man: $(MANPAGES)
	-mkdir $(MANDIR)
	@for i in $(MANPAGES); do \
		echo Installing manual page for $$i; \
		$(INSTALLMAN) $$i $(MANDIR)/$$i; \
	done

