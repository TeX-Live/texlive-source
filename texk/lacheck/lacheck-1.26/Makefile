# Makefile for lacheck.

# If both flex lacheck.c are missing, copy lacheck.noflex.c to
# lacheck.c. 

prefix = /usr/local
exec_prefix = $(prefix)

bindir = $(exec_prefix)/bin
mandir = $(prefix)/man/man1
manext = .1
srcdir = .

# Use `cc -bsd' on a next
# use `cc -cckr' on a sgi
# CC = gcc -traditional
CC=cc

CFLAGS = -g # -O -DNEED_STRSTR 

# No changes should be needed below

# Remember to change when lacheck is updated.
REV = 1.26

DISTFILES = lacheck.lex lacheck.man Makefile lacheck.c test.tex test.old \
	lacheck.hlp make_vms.com

FTPDIR = /home/ftp/pub/Staff/Per.Abrahamsen/mirror/ftp/lacheck

SHELL = /bin/sh

LACHECK=lacheck

INSTALL = cp
INSTALL_PROGRAM = $(INSTALL) 
INSTALL_DATA = $(INSTALL) 

FLEX = flex
FLEXFLAGS = -8

LIBS= #-ll

$(LACHECK):	lacheck.o
	$(CC) $(CFLAGS) -o $@ $(srcdir)/lacheck.o $(LIBS)

lacheck.c:	lacheck.lex  
	-if [ ! -f $(srcdir)/lacheck.noflex.c ] ; then \
	  if [ -f $(srcdir)/lacheck.c ] ; then \
	    $(INSTALL_DATA) $(srcdir)/lacheck.c $(srcdir)/lacheck.noflex.c ; \
	  fi ; \
	fi
	$(FLEX) $(FLEXFLAGS) $(srcdir)/lacheck.lex
	mv $(srcdir)/lex.yy.c $@

lacheck.1: 	lacheck.man  
	-sed -e "s!%%LACHECKREV%%!Release $(REV)!" $(srcdir)/lacheck.man |\
	sed -e "s!%%LACHECKDATE%%!`date +%D`!" |\
	sed -e "s!%%LACHECKPATH%%!$(bindir)/$(LACHECK)!" > $@ 

test.new:	test.tex $(LACHECK) 
	$(srcdir)/$(LACHECK) $(srcdir)/test.tex > $@ 

test.old:	test.tex $(LACHECK)
	$(srcdir)/$(LACHECK) $(srcdir)/test.tex > $@ 

check:		test.new
	-diff $(srcdir)/test.old $(srcdir)/test.new

install: 	$(LACHECK) lacheck.1
	cp $(srcdir)/$(LACHECK) $(bindir)
	cp $(srcdir)/lacheck.1 $(mandir)/lacheck$(manext) 

uninstall:
	-rm -f $(bindir)/$(LACHECK) $(mandir)/lacheck$(manext) 

clean:
	-rm -f $(srcdir)/$(LACHECK) $(srcdir)/*.o $(srcdir)/*.1 $(srcdir)/*~

distclean:	clean

mostlyclean:	clean
	-rm -f $(srcdir)/lacheck.c $(srcdir)/lacheck.noflex.c

realclean:	mostlyclean
	-rm -f test.old

dist:	$(DISTFILES) 
	mkdir lacheck-$(REV) 
	cp $(DISTFILES) lacheck-$(REV)
	touch lacheck-$(REV)/lacheck.c
	-rm -f $(FTPDIR)/lacheck-$(REV).tar.gz
	-rm -f $(FTPDIR)/lacheck.tar.gz $(FTPDIR)/lacheck.tar.Z
	tar -cf - lacheck-$(REV) | gzip > $(FTPDIR)/lacheck-$(REV).tar.gz
	tar -cf - lacheck-$(REV) | compress > $(FTPDIR)/lacheck-$(REV).tar.Z
	(cd $(FTPDIR); ln -s lacheck-$(REV).tar.gz lacheck.tar.gz)

