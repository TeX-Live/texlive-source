# Makefile fragment for MetaPost.
# $Id: metapost.mk,v 1.15 2005/07/04 07:51:41 taco Exp $
#
# Public domain. 
#
# This facilitates a separate development track for MetaPost.

Makefile: metapost.mk

mpost = mpost
metapost = $(mpost) dvitomp
mpostdir = mpdir

# mpware is the subdirectory.
mpware = mpware
mpware_programs = mpware/dmp mpware/makempx mpware/mpto mpware/newer
mpware_sources = mpware/dmp.c mpware/makempx.in mpware/mpto.c mpware/newer.c

# mpost

mp_c = mpini.c mp0.c mp1.c mp2.c
mp_o = mpini.o mp0.o mp1.o mp2.o mpextra.o mp-loadpool.o

mp-loadpool.c: mp.pool $(makecpool)
	$(makecpool) mp.pool mpdir/mplib.h >$@ || rm -f $@
# mpostlibsdep also includes makecpool
mpost: $(mp_o) $(mpostlibsdep)
	$(kpathsea_link) $(mp_o) $(mpostlibs) $(LOADLIBES)
$(mp_c) mpcoerce.h mpd.h: mp.p $(web2c_texmf) $(srcdir)/$(mpostdir)/mp.defines  web2c/cvtmf1.sed web2c/cvtmf2.sed
	$(web2c) mp
mpextra.c: lib/texmfmp.c
	sed s/TEX-OR-MF-OR-MP/mp/ $(srcdir)/lib/texmfmp.c >$@
mp.p mp.pool: tie tangle mp.web mp.ch
	$(tangle) $(srcdir)/mp.web $(srcdir)/mp.ch
check: mpost-check
mpost-check: mptrap mpost.mem $(mpware)
	./mpost --progname=mpost '&./mpost \tracingstats:=1; end.'
	TEXMFCNF=../kpathsea \
	  MAKEMPX_BINDIR=`pwd`:`pwd`/mpware MPXCOMMAND=mpware/makempx \
	  ./mpost --progname=mpost $(srcdir)/tests/mptest
	./mpost --progname=mpost $(srcdir)/tests/one.two
	./mpost --progname=mpost $(srcdir)/tests/uno.dos
clean:: mpost-clean
mpost-clean: mptrap-clean
	$(LIBTOOL) --mode=clean rm -f mpost
	rm -f $(mp_o) $(mp_c) mpextra.c mpcoerce.h mpd.h mp.p mp.pool
	rm -f mpost.mem mpost.log
	rm -f mpout.log mptest.log one.two.log uno.log

# Can't run trap and mptrap in parallel, because both write trap.{log,tfm}.
mptrap: mpost pltotf tftopl mptrap-clean
	@echo ">>> See $(testdir)/mptrap.diffs for example of acceptable diffs." >&2
	$(LN) $(testdir)/mtrap.mp . # get same filename in log
	./pltotf $(testdir)/trapf.pl trapf.tfm
	-$(SHELL) -c '$(testenv) ./mpost --progname=inimpost mtrap'
	-diff $(testdir)/mtrap.log mtrap.log
	-diff $(testdir)/mtrap.0 mtrap.0
	-diff $(testdir)/mtrap.1 mtrap.1
	-diff $(testdir)/writeo writeo
	-diff $(testdir)/writeo.2 writeo.2
	$(LN) $(testdir)/trap.mp .
	$(LN) $(testdir)/trap.mpx .
	-$(SHELL) -c '$(testenv) ./mpost --progname=inimpost <$(testdir)/mptrap1.in >mptrapin.fot'
	mv trap.log mptrapin.log
	-diff $(testdir)/mptrapin.log mptrapin.log
# Must run inimp or font_name[null_font] is not initialized, leading to diffs.
	-$(SHELL) -c '$(testenv) ./mpost --progname=inimpost <$(testdir)/mptrap2.in >mptrap.fot'
	mv trap.log mptrap.log
	mv trap.tfm mptrap.tfm
	-diff $(testdir)/mptrap.fot mptrap.fot
	-diff $(testdir)/mptrap.log mptrap.log
	-diff $(testdir)/trap.5 trap.5
	-diff $(testdir)/trap.6 trap.6
	-diff $(testdir)/trap.148 trap.148
	-diff $(testdir)/trap.149 trap.149
	-diff $(testdir)/trap.150 trap.150
	-diff $(testdir)/trap.151 trap.151
	-diff $(testdir)/trap.197 trap.197
	-diff $(testdir)/trap.200 trap.200
	./tftopl ./mptrap.tfm mptrap.pl
	-diff $(testdir)/mptrap.pl mptrap.pl

mptrap-clean:
	rm -f mtrap.mp mtrap.mem trapf.tfm
	rm -f mtrap.log mtrap.0 mtrap.1 writeo writeo.log writeo.2
	rm -f trap.mp trap.mpx mptrapin.fot mptrapin.log
	rm -f mptrap.fot mptrap.log mptrap.tfm
	rm -f trap.ps trap.mem trap.0 trap.5 trap.6 trap.95 trap.96 trap.97
	rm -f trap.98 trap.99 trap.100 trap.101 trap.102 trap.103 trap.104
	rm -f trap.105 trap.106 trap.107 trap.108 trap.109 trap.148
	rm -f trap.149 trap.150 trap.151 trap.197 trap.200
	rm -f mptrap.pl


# mpware

# We put mpware (written directly in C) in a subdirectory.
$(mpware): $(mpware_programs)

$(mpware_programs): $(mpware_sources)
	cd $(mpware) && $(MAKE) $(common_makeargs)

install-programs: install-mpware-programs
install-mpware-programs: $(mpware_programs)
	cd $(mpware) && $(MAKE) $(install_makeargs) install-exec

mp-programs: $(metapost) $(mpware)
