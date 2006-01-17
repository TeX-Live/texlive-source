# Makefile fragment for DVItoMP.
# $Id: dvitomp.mk,v 1.1 2005/06/21 16:03:15 olaf Exp $
#
# Public domain.
#
# This facilitates a separate development track for DVItoMP.

Makefile: dvitomp.mk

dvitomp: dvitomp.o
	$(kpathsea_link) dvitomp.o $(LOADLIBES)
dvitomp.c dvitomp.h: $(web2c_common) $(web2c_programs) dvitomp.p
	$(web2c) dvitomp
dvitomp.p: tangle dvitomp.web dvitomp.ch
	$(tangle) dvitomp dvitomp
check: dvitomp-check
dvitomp-check: dvitomp
	TEXMFCNF=../kpathsea \
	  ./dvitomp $(srcdir)/tests/story.dvi tests/xstory.mpx
	TFMFONTS=$(srcdir)/tests VFFONTS=$(srcdir)/tests: \
	  ./dvitomp $(srcdir)/tests/ptmr 
	mv ptmr.mpx tests/xptmr.mpx
clean:: dvitomp-clean
dvitomp-clean:
	$(LIBTOOL) --mode=clean rm -f dvitomp
	rm -f dvitomp.o dvitomp.c dvitomp.h dvitomp.p
	rm -f tests/xstory.mpx tests/xptmr.mpx
