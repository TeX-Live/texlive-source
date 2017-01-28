# This file is part of CWEB.
# It is distributed WITHOUT ANY WARRANTY, express or implied.
# Version 3.64 --- February 2002

# Copyright (C) 1987,1990,1993,2000 Silvio Levy and Donald E. Knuth

# Permission is granted to make and distribute verbatim copies of this
# document provided that the copyright notice and this permission notice
# are preserved on all copies.

# Permission is granted to copy and distribute modified versions of this
# document under the conditions for verbatim copying, provided that the
# entire resulting derived work is given a different name and distributed
# under the terms of a permission notice identical to this one.

# 
# Read the README file, then edit this file to reflect local conditions
#

# directory for TeX inputs (cwebmac.tex goes here)
MACROSDIR= /usr/share/texmf/tex/generic

# directory for CWEB inputs in @i files
CWEBINPUTS= /usr/local/lib/cweb

# extension for manual pages ("l" distinguishes local from system stuff)
MANEXT= l
#MANEXT= 1

# directory for manual pages (cweb.1 goes here)
MANDIR= /usr/share/man/man$(MANEXT)

# destination directory for executables; must end in /
DESTDIR= /usr/local/bin/

# directory for GNU EMACS Lisp code (cweb.el goes here)
EMACSDIR= /usr/share/emacs/site-lisp

# Set DESTPREF to null if you want to call the executables "tangle" and "weave"
# (probably NOT a good idea; we recommend leaving DESTPREF=c)
DESTPREF=c

# Set CCHANGES to comm-foo.ch if you need changes to common.w
CCHANGES=

# Set TCHANGES to ctang-foo.ch if you need changes to ctangle.w
TCHANGES=

# Set WCHANGES to cweav-foo.ch if you need changes to cweave.w
WCHANGES=

# We keep debugging info around, for fun, but most users don't need it
CFLAGS = -g
#CFLAGS = -O
LINKFLAGS = -g
#LINKFLAGS = -s # for smaller (stripped) executables on many UNIX systems

# What C compiler are you using?
CC = cc

# RM and CP are used below in case rm and cp are aliased
RM= /bin/rm
CP= /bin/cp

# uncomment the second line if you use pdftex to bypass .dvi files
PDFTEX = dvipdfm
#PDFTEX = pdftex

##########  You shouldn't have to change anything after this point #######

CWEAVE = ./cweave
CTANGLE = ./ctangle
SOURCES = cweave.w common.w ctangle.w
ALL =  common.w ctangle.w cweave.w prod.w \
	Makefile common.c common.h ctangle.c \
	cwebman.tex cwebmac.tex comm-vms.ch ctang-vms.ch \
	cweav-vms.ch comm-man.ch ctang-man.ch cweav-man.ch \
	comm-pc.ch ctang-pc.ch cweav-pc.ch comm-amiga.ch \
        comm-bs.ch ctang-bs.ch cweav-bs.ch makefile.bs \
	comm-ql.ch ctang-ql.ch cweav-ql.ch readme.ql \
	comm-w32.ch ctang-w32.ch cweav-w32.ch \
	comm-os2.ch comm-mac.ch cweb.1 cweb.el c++lib.w README

.SUFFIXES: .dvi .tex .w .pdf

.w.tex:
	$(CWEAVE) $*

.tex.dvi:	
	tex $<

.w.dvi:
	make $*.tex
	make $*.dvi

.w.c:
	$(CTANGLE) $*

.w.o:
	make $*.c
	make $*.o

.w.pdf:
	make $*.tex
	case "$(PDFTEX)" in \
	 dvipdfm ) tex "\let\pdf+ \input $*"; dvipdfm $* ;; \
	 pdftex ) pdftex $* ;; \
	esac

all: ctangle cweave

cautiously: ctangle
	$(CP) common.c SAVEcommon.c
	./ctangle common $(CCHANGES)
	diff common.c SAVEcommon.c
	$(RM) SAVEcommon.c
	$(CP) ctangle.c SAVEctangle.c
	./ctangle ctangle $(TCHANGES)
	diff ctangle.c SAVEctangle.c
	$(RM) SAVEctangle.c

SAVEctangle.c:
	$(CP) ctangle.c SAVEctangle.c

SAVEcommon.c:
	$(CP) common.c SAVEcommon.c

common.c: common.w $(CCHANGES)
	$(CTANGLE) common $(CCHANGES)

common.o: common.c
	$(CC) $(CFLAGS) -DCWEBINPUTS=\"$(CWEBINPUTS)\" -c common.c

ctangle: ctangle.o common.o
	$(CC) $(LINKFLAGS) -o ctangle ctangle.o common.o 

ctangle.c: ctangle.w $(TCHANGES)
	$(CTANGLE) ctangle $(TCHANGES)

cweave: cweave.o common.o
	$(CC) $(LINKFLAGS) -o cweave cweave.o common.o

cweave.c: cweave.w $(WCHANGES)
	$(CTANGLE) cweave $(WCHANGES)

doc: $(SOURCES:.w=.dvi)

usermanual: cwebman.tex cwebmac.tex
	tex cwebman

fullmanual: usermanual $(SOURCES) comm-man.ch ctang-man.ch cweav-man.ch
	make cweave
	./cweave common.w comm-man.ch
	tex common.tex
	./cweave ctangle.w ctang-man.ch
	tex ctangle.tex
	./cweave cweave.w cweav-man.ch
	tex cweave.tex

# be sure to leave ctangle.c and common.c for bootstrapping
clean:
	$(RM) -f -r *~ *.o common.tex cweave.tex cweave.c ctangle.tex \
	  *.log *.dvi *.toc *.idx *.scn *.pdf core cweave ctangle

install: all
	- mkdir $(DESTDIR)
	$(CP) cweave $(DESTDIR)$(DESTPREF)weave
	chmod 755 $(DESTDIR)$(DESTPREF)weave
	$(CP) ctangle $(DESTDIR)$(DESTPREF)tangle
	chmod 755 $(DESTDIR)$(DESTPREF)tangle
	- mkdir $(MANDIR)
	$(CP) cweb.1 $(MANDIR)/cweb.$(MANEXT)
	chmod 644 $(MANDIR)/cweb.$(MANEXT)
	- mkdir $(MACROSDIR)
	$(CP) cwebmac.tex $(MACROSDIR)
	chmod 644 $(MACROSDIR)/cwebmac.tex
	- mkdir $(EMACSDIR)
	$(CP) cweb.el $(EMACSDIR)
	chmod 644 $(EMACSDIR)/cweb.el
	- mkdir $(CWEBINPUTS)
	$(CP) c++lib.w $(CWEBINPUTS)
	chmod 644 $(CWEBINPUTS)/c++lib.w

floppy: $(ALL) examples
	bar cvhf /dev/rfd0 $(ALL) examples
	bar tvf /dev/rfd0
	eject

tags: $(ALL)
	etags -lnone $(ALL)

tarfile: $(ALL) examples
	tar cvhf /tmp/cweb.tar $(ALL) examples
	gzip -9 /tmp/cweb.tar

tarball:
	tar zcvhf /tmp/cweb.tgz $(ALL) examples
