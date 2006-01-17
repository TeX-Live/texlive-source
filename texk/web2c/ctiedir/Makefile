#
#   Makefile for ctie distribution		-	21 Dec 92
#
#   This file makes CTIE and needs ctangle from cweb distribution.
#

CTANGLE=ctangle
BINDIR=.
CTIE=$(BINDIR)/ctie

# directory for CWEB inputs in @i files
CWEBINPUTS= /usr/local/lib/cweb

CFLAGS=
LDFLAGS=
KPATHSEAFLAGS=-lkpathsea

default: ctie


# CTIE
ctie.o: ctie.c
	$(CC) $(LDFLAGS) $(CFLAGS) -DCWEBINPUTS=\"$(CWEBINPUTS)\" -c ctie.c

ctie: ctie.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $(CTIE) ctie.o

ctie.c: ctie.w
	$(CTANGLE) ctie

ctie-k.o: ctie-k.c

ctie-k: ctie-k.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $(BINDIR)/ctie-k ctie-k.o $(KPATHSEAFLAGS)

ctie-k.c: ctie.w ctie-k.ch
	ctangle ctie.w ctie-k.ch ctie-k.c

clean:
	rm -f ctie.o ctie-k.o ctie.outc ctie.outm ctie.tex
	rm -f ctie.log ctie.toc ctie.scn ctie.idx
	rm -f ctiedoc.aux ctiedoc.log

veryclean: clean
	rm -f $(BINDIR)/ctie $(BINDIR)/ctie-k ctie.dvi ctiedoc.dvi

ttest:  $(CTIE)
	$(CTIE) -c ctie.outc ctie.tie ctie.cf?
	diff ctie.outc ctie.chf.gen
	$(CTIE) -m ctie.outm ctie.tie ctie.cf?
	diff ctie.outm ctie.master.gen
