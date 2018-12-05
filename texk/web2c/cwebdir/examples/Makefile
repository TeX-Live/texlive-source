#
# generic Makefile to use when you've got CWEB
# (add it to, or add to it, whatever you like)
#
#======================================================================

CFLAGS = -g
MLIB = -lm
RM = /bin/rm -f
WFILES = extex.w treeprint.w wc.w wmerge.w wordtest.w oemacs.w

#======================================================================

.SUFFIXES: .tex .dvi .w

.w.tex:
	cweave $*

.tex.dvi:	
	tex $<

.w.dvi:
	make $*.tex
	make $*.dvi

.w.c:
	ctangle $*

.w.o:
	make $*.c
	make $*.o

.c.o:
	cc $(CFLAGS) -c $*.c

.w:
	make $*.c
	cc $(CFLAGS) $*.c -o $*

#======================================================================

all: $(WFILES:.w=) excweb

excweb: extex
	ln extex excweb

oemacs: oemacs.c
	cc $(CFLAGS) -I/usr/openwin/include oemacs.c \
             -o oemacs -lxview -lolgx -lX11

doc: $(WFILES:.w=.dvi)

clean:
	$(RM) *~ \#~ .\#* *.o *.log *.dvi *.toc core a.out

clobber: clean
	$(RM) $(WFILES:.w=) $(WFILES:.w=.c) $(WFILES:.w=.tex)
