#
# Makefile
# by pts@fazekas.hu at Sun Feb 24 16:20:15 CET 2002
# Sat Apr 20 19:14:11 CEST 2002
# ccdep.pl at Sat Jun  1 15:51:36 CEST 2002
#
# To create a .tar.gz, run: make MAKE_DIST=0 dist-noautoconf
# To install dist: make MAKE_DIST=0 dist-install
#

ifndef MAKE_DIST
include Makehelp
endif

BASH=bash
PERL_BADLANG=x
export PERL_BADLANG

CXXX=$(CXD_assert)
LDALL=$(LDXX) -s $(LDFLAGS) $(LIBS)
ifeq ($(ENABLE_DEBUG), no)
CXXX=$(CXD_no)
endif
ifeq ($(ENABLE_DEBUG), yes)
CXXX=$(CXD_yes)
LDALL=$(LDXX) $(LDFLAGS) $(LIBS)
endif
ifeq ($(ENABLE_DEBUG), checker)
CXXX=$(CXD_checker)
LDALL=checkerg++ $(LDFLAGS) $(LIBS)
endif

CXXALL=$(CXXX) $(CXXFLAGS) $(CXXFLAGSB)
CCALL =$(CXXX) $(CXXFLAGS) $(CXXFLAGSB)
CXDFAL=$(CXXFLAGS) $(CXXFLAGSB) $(LDFLAGS) $(LIBS)

.PHONY: clean dist dist-noautoconf dist-more allclean distclean dist-install
.PHONY: most all all1 install

ifndef MAKE_DIST
  most all: sam2p
  Makedep: ccdep.pl config.h; perl -x -S ./ccdep.pl --FAL=assert,no,yes,checker $(CXX)
  include Makedep
endif

all1: $(ALL)
# vvv for Epsilon at Thu Oct 31 09:58:58 CET 2002
IDE_MODES := release debug
IDE_TARGETS_debug := $(patsubst %,%.yes, $(TARGETS))
IDE_TARGETS_release := $(TARGETS)

CXD_assert =$(CXX) -s -O2
CXD_no     =$(CXX) -s -DNDEBUG -O3
# CXD_yes    =$(CXX) $(GFLAG) -lefence
CXD_yes    =$(CXX) $(GFLAG)
CXD_checker=checkerg++ $(GFLAG)

# .PHONY: clean dist allclean distclean

Makedep: config.h

xpmc.h: cols2.pl
	perl -x cols2.pl >xpmc.h

#HQ=perl -e'$$_=join"",<STDIN>; s@([^\w\.\/\-])@sprintf"\\%03o",ord$$1@ge; print "\"$$_\"\n"'
HQ=perl -x hq.pl
#%.tth: %.tte
#	<$< >$@	$(HQ)
#%.tth: %.ttm
#	<$< >$@	$(HQ)
%.tth: %.ttt
	$(HQ) <$< >$@

# Sun Sep 22 01:16:20 CEST 2002
L1_LIST=l1g8z.pst l1ghz.pst l1gbz.pst \
        l1g8l.pst l1ghl.pst l1gbl.pst
PREPROC_STRIP=perl -ne's@/\s+(?=\w)@/@g;print if!/^\#/&&!/^\s*\Z/'
L1_FLAGS=-DCFG_FMT_ZLIB_ONLY=1 -DNDEBUG=1 \
  -DCFG_NO_VAR_S=1 -DUSE_NO_BIND=1 -DUSE_SHORT_NAMES=1 \
  -DUSE_CURRENTFILE=1 -DUSE_NO_EOF=1 \
  -DUSE_UNITLENGTH_8 -DUSE_EARLYCHANGE_1 -DUSE_LOWBITFIRST_FALSE \
  -DUSE_NO_NULLDEF=1 -DUSE_PIN=1
# TTT_QUOTE=perl -e '$$s=$$_=join"",<STDIN>; s@%.*@@g; s@[(][^()]*[)]@@g; die if /[()\\]/ or $$ARGV[0]!~/^(\w+)/; print "\n% TTT_QUOTE\n/$$1 ($$s)\n\n"' --
TTT_QUOTE=perl -e '$$s=$$_=join"",<STDIN>; $$s=~s@([()\\])@\\$$1@g; die if $$ARGV[0]!~/^(\w+)/; print "\n% TTT_QUOTE\n/$$1 ($$s)\n\n"' --
# vvv Dat: input file for `g++ -E' must have .h extension
l1g8z.pst: l1zip.psm psmlib.psm ps_tiny
	<$< >tmp.h perl -pe0
	$(CXX) -E $(L1_FLAGS) -DUSE_A85D=1 tmp.h >tmp.i
	<tmp.i >tmp.pin $(PREPROC_STRIP)
	<tmp.pin >tmp.ps0 ./ps_tiny
	<tmp.ps0 >tmp.pst $(TTT_QUOTE) $@
	mv -f tmp.pst $@
l1ghz.pst: l1zip.psm psmlib.psm ps_tiny
	<$< >tmp.h perl -pe0
	$(CXX) -E $(L1_FLAGS) -DUSE_HEXD=1 tmp.h >tmp.i
	<tmp.i >tmp.pin $(PREPROC_STRIP)
	<tmp.pin >tmp.ps0 ./ps_tiny
	<tmp.ps0 >tmp.pst $(TTT_QUOTE) $@
	mv -f tmp.pst $@
l1gbz.pst: l1zip.psm psmlib.psm ps_tiny
	<$< >tmp.h perl -pe0
	$(CXX) -E $(L1_FLAGS) -DUSE_BINARY=1 tmp.h >tmp.i
	<tmp.i >tmp.pin $(PREPROC_STRIP)
	<tmp.pin >tmp.ps0 ./ps_tiny
	<tmp.ps0 >tmp.pst $(TTT_QUOTE) $@
	mv -f tmp.pst $@
l1g8l.pst: l1lzw.psm psmlib.psm ps_tiny
	<$< >tmp.h perl -pe0
	$(CXX) -E $(L1_FLAGS) -DUSE_A85D=1 tmp.h >tmp.i
	<tmp.i >tmp.pin $(PREPROC_STRIP)
	<tmp.pin >tmp.ps0 ./ps_tiny
	<tmp.ps0 >tmp.pst $(TTT_QUOTE) $@
	mv -f tmp.pst $@
l1ghl.pst: l1lzw.psm psmlib.psm ps_tiny
	<$< >tmp.h perl -pe0
	$(CXX) -E $(L1_FLAGS) -DUSE_HEXD=1 tmp.h >tmp.i
	<tmp.i >tmp.pin $(PREPROC_STRIP)
	<tmp.pin >tmp.ps0 ./ps_tiny
	<tmp.ps0 >tmp.pst $(TTT_QUOTE) $@
	mv -f tmp.pst $@
l1gbl.pst: l1lzw.psm psmlib.psm ps_tiny
	<$< >tmp.h perl -pe0
	$(CXX) -E $(L1_FLAGS) -DUSE_BINARY=1 tmp.h >tmp.i
	<tmp.i >tmp.pin $(PREPROC_STRIP)
	<tmp.pin >tmp.ps0 ./ps_tiny
	<tmp.ps0 >tmp.pst $(TTT_QUOTE) $@
	mv -f tmp.pst $@
# vvv copy the .ttt, not the .tth
#     The perl program down there is a portable cat(1) implementation.
bts1.ttt: bts.ttt $(L1_LIST)
	perl -pe0 $^ >$@
bts2.ttt: bts1.ttt ps_tiny
	./ps_tiny --copy <$< >$@

clean:
	rm -f *~ a.out DEADJOE core *.o *.tth .rgd *.rgd tmp.pin tmp.i tmp.ps0 tmp.h tmp.pst autom4te.cache/*
	rm -f debian/changelog.dch debian/*~ 
	rm -f $(ALL) $(ALL:=.yes) $(ALL:=.no) $(ALL:=.assert) $(ALL:=.checker)
	-rmdir -- autom4te.cache
allclean: clean
	rm -f configure config.h Makehelp config.cache config.log \
	  config.status test.eps test.pdf build build-stamp
distclean: allclean
	-autoconf
dist: distclean dist-noautoconf
dist-noautoconf:
	chmod 755 configure
	$(BASH) mkdist.sh
# vvv Create a distribution with more files to aid compilation
dist-more: bts2.tth
	$(BASH) mkdist.sh sam2p-more bts2.tth
dist-install: dist-noautoconf
	chmod 600 ../sam2p-*.tar.gz
	scp ../sam2p-*.tar.gz kozma:public_html

install: sam2p
	-mkdir -p '$(bindir)'
	cp -a sam2p '$(bindir)'

# __END__ of Makefile
