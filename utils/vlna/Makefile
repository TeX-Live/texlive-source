
DESTDIR=/usr/local
MANEXT=1
BINDIR=$(DESTDIR)/bin
MANDIR=$(DESTDIR)/man/man$(MANEXT)

CC=cc
INSTALL=install
MKDIR=mkdir -p
INSTPROGFLAGS=-s -m755
INSTDATAFLAGS=-m644

CFLAGS=-O
LFLAGS=

CTANGLE=ctangle
CWEAVE=cweave
TEX=csplain

all: vlna

vlna: vlna.c
	$(CC) $(CFLAGS) $(LFLAGS) $? -o $@

install: all install.man
	@if [ -d $(BINDIR) ]; then \
		set +x; else (set -x; $(MKDIR) $(BINDIR)); fi
	$(INSTALL) $(INSTPROGFLAGS) vlna $(BINDIR)/vlna

install.man:
	sed -e "s|@BINDIR@|$(BINDIR)|;\
		s|@MANDIR@|$(MANDIR)|;\
		s|@MANEXT@|$(MANEXT)|" vlna.man > vlna.$(MANEXT)
	@if [ -d $(MANDIR) ]; then \
		set +x; else (set -x; $(MKDIR) $(MANDIR)); fi
	$(INSTALL) $(INSTDATAFLAGS) vlna.$(MANEXT) $(MANDIR)/vlna.$(MANEXT)

vlna.c: vlna.w
	$(CTANGLE) $? - $@

vlna.tex: vlna.w
	$(CWEAVE) $? - $@

vlna.dvi: vlna.tex
	$(TEX) $?

clean:
	rm -f *.o *~ *.bak core a.out vlna vlna.$(MANEXT)
	rm -f vlna.idx vlna.scn vlna.toc vlna.log vlna.tex

cleanall: clean
	rm -f vlna.c vlna.dvi

