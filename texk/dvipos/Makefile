# Generated automatically from Makefile.in by configure.
# Makefile.in generated automatically by automake 1.4-p5 from Makefile.am

# Copyright (C) 1994, 1995-8, 1999, 2001, 2007 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.


SHELL = /bin/sh

# UNIX/Linux (check /usr/local/share/texmf)
prefix = /usr/local
# SUN Solaris (check /usr/local/teTeX)
#prefix = /usr/local/teTeX
# CYGWIN (check /usr/share/texmf)
#prefix = /usr
exec_prefix = ${prefix}

bindir = ${exec_prefix}/bin
libdir = ${exec_prefix}/lib
mandir = ${prefix}/man
includedir = ${prefix}/include

INSTALL = /usr/bin/install -c
transform = s,x,x,

NORMAL_INSTALL = :
NORMAL_UNINSTALL = :
CC = gcc
LN_S = ln -s
PACKAGE = dvipos
VERSION = 20070107

PROGRAMS = dvipos
SOURCES = dvicore.c dvipos.c tfm.c utils.c dvicore.h tfm.h utils.h

#mkinstalldirs = $(SHELL) ./mkinstalldirs

DEFS = -DKPATHSEA -I.
CPPFLAGS = -I${includedir}
LDFLAGS = -L${libdir}
kpathsea = ../kpathsea/.libs/libkpathsea.a
LIBS = ${kpathsea} -lm
OBJECTS = dvicore.o dvipos.o tfm.o utils.o

CFLAGS = -g -O2
COMPILE = $(CC) $(DEFS) $(CPPFLAGS) $(CFLAGS)
LINK = $(CC) $(CFLAGS) $(LDFLAGS) -o $@

all: $(PROGRAMS)
.SUFFIXES: .c .o

clean:
	-rm -f *.o core *.core
	-test -z "$(PROGRAMS)" || rm -f $(PROGRAMS)

install: $(PROGRAMS)
	@$(NORMAL_INSTALL)
#	$(mkinstalldirs) $(DESTDIR)$(bindir)
	@list='$(PROGRAMS)'; for p in $$list; do \
	  if test -f $$p; then \
	    echo "  $(INSTALL) $$p $(bindir)/`echo $$p|sed 's/$(EXEEXT)$$//'|sed '$(transform)'|sed 's/$$/$(EXEEXT)/'`"; \
	     $(INSTALL) $$p $(bindir)/`echo $$p|sed 's/$(EXEEXT)$$//'|sed '$(transform)'|sed 's/$$/$(EXEEXT)/'`; \
	  else :; fi; \
	done

install-man: dvipos.1
	$(INSTALL) dvipos.1 $(mandir)/man1/

uninstall:
	@$(NORMAL_UNINSTALL)
	list='$(PROGRAMS)'; for p in $$list; do \
	  rm -f $(bindir)/`echo $$p|sed 's/$(EXEEXT)$$//'|sed '$(transform)'|sed 's/$$/$(EXEEXT)/'`; \
	done

uninstall-man:
	rm -f $(mandir)/man1/dvipos.1

.c.o:
	$(COMPILE) -c $<

dvipos: $(OBJECTS)
	@rm -f dvipos
	$(LINK) $(LDFLAGS) $(OBJECTS) $(LDADD) $(LIBS)

dvipos.o: dvipos.c dvicore.h utils.h
dvicore.o: dvicore.c dvicore.h tfm.h utils.h
tfm.o: tfm.c tfm.h dvicore.h utils.h
utils.o: utils.c utils.h
