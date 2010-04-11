# *ATTENTION* : This file is not used in TeX Live 2009. See am/ptex.am.
# Makefile fragment for pTeX and web2c. -- tutimura(a)nn.iij4u.or.jp. Public domain.
# This fragment contains the parts of the makefile that are most likely to
# differ between releases of pTeX and derived from e-TeX.

Makefile: $(srcdir)/ptexdir/ptex.mk

# We build ptex.
ptex = @JPTEX@ ptex pbibtex ptftopl ppltotf pdvitype

# Extract ptex version
ptexdir/ptex.version: ptexdir/ptex-base.ch
	grep "^@d TeX_banner=='This is pTeX" $(srcdir)/ptexdir/ptex-base.ch \
	  | sed "s/^.*-//;s/'.*$$//" >ptexdir/ptex.version

# The C sources.
ptex_c = ptexini.c ptex0.c ptex1.c ptex2.c
ptex_o = ptexini.o ptex0.o ptex1.o ptex2.o ptex-pool.o ptexextra.o ptexdir/kanji.o
plib_o = ptexdir/printversion.o ptexdir/usage.o ptexdir/openclose.o
plib   = ptexdir/plib.a

# Making ptex.
ptex: $(ptex_o) $(plib) $(ptexenc)
	$(kpathsea_link) $(ptex_o) $(plib) $(ptexenc) $(socketlibs) $(LOADLIBES)

# C file dependencies
$(ptex_c) ptexcoerce.h ptexd.h: ptex.p $(web2c_texmf)
	$(web2c) ptex
ptexextra.c: ptexdir/ptexextra.h lib/texmfmp.c
	sed s/TEX-OR-MF-OR-MP/ptex/ $(srcdir)/lib/texmfmp.c >$@
ptexdir/ptexextra.h: ptexdir/ptexextra.in ptexdir/ptex.version
	test -d ptexdir || mkdir ptexdir
	sed s/PTEX-VERSION/`cat ptexdir/ptex.version`/ \
          $(srcdir)/ptexdir/ptexextra.in >$@

# Tangling
ptex.p ptex.pool: tangle ptex.web ptex.ch
	$(TANGLE) ptex.web ptex.ch

# Generation of the web and ch file.
#   Sources for ptex.web:
ptex_web_srcs = $(srcdir)/tex.web \
  $(srcdir)/tex.ch
#   Sources for ptex.ch:
ptex_ch_srcs = ptex.web \
  $(srcdir)/ptexdir/ptex-base.ch \
  $(srcdir)/ptexdir/ptex-include.ch \
  $(srcdir)/tex-binpool.ch
#   Rules:
ptex.web: tie $(srcdir)/ptexdir/ptex.mk $(ptex_web_srcs)
	$(TIE) -m ptex.web $(ptex_web_srcs)
ptex.ch: $(ptex_ch_srcs)
	$(TIE) -c ptex.ch $(ptex_ch_srcs)

ptex-pool.c: ptex.pool $(makecpool) tmf-pool.h
	$(makecpool) ptex.pool $(srcdir)/tmf-pool.h >$@ || rm -f $@

$(plib): $(plib_o)
	rm -f $@
	$(AR) $(ARFLAGS) $@ $(plib_o)
	$(RANLIB) $@

### pTFtoPL
ptftopl: ptftopl.o $(plib) $(kpathsea) $(ptexenc) $(proglib)
	$(kpathsea_link) ptftopl.o $(plib) $(ptexenc) $(LOADLIBES)
ptftopl.o: ptftopl.c $(srcdir)/ptexdir/kanji.h $(srcdir)/ptexdir/ptexhelp.h
ptftopl.c: ptftopl.p $(web2c_aux)
	$(web2c) ptftopl
ptftopl.p:        ptftopl.web $(srcdir)/ptexdir/ptftopl.ch
	$(tangle) ptftopl.web $(srcdir)/ptexdir/ptftopl.ch
ptftopl.web:                  $(srcdir)/tftopl.web $(srcdir)/tftopl.ch
	$(TIE) -m ptftopl.web $(srcdir)/tftopl.web $(srcdir)/tftopl.ch


### pPLtoTF
ppltotf: ppltotf.o $(plib) $(kpathsea) $(ptexenc) $(proglib)
	$(kpathsea_link) ppltotf.o $(plib) $(ptexenc) $(LOADLIBES)
ppltotf.o: ppltotf.c $(srcdir)/ptexdir/kanji.h $(srcdir)/ptexdir/ptexhelp.h
	$(compile) -DPRESERVE_PUTC -c ppltotf.c
ppltotf.c: ppltotf.p $(web2c_aux)
	$(web2c) ppltotf
ppltotf.p: tangle ppltotf.web $(srcdir)/ptexdir/ppltotf.ch
	$(tangle) ppltotf.web $(srcdir)/ptexdir/ppltotf.ch
ppltotf.web:                  $(srcdir)/pltotf.web $(srcdir)/pltotf.ch
	$(TIE) -m ppltotf.web $(srcdir)/pltotf.web $(srcdir)/pltotf.ch

### pDVItype
pdvitype: pdvitype.o $(plib) $(kpathsea) $(ptexenc) $(proglib)
	$(kpathsea_link) pdvitype.o $(plib) $(ptexenc) $(LOADLIBES)
pdvitype.o: pdvitype.c $(srcdir)/ptexdir/kanji.h $(srcdir)/ptexdir/ptexhelp.h
	$(compile) -DHEX_CHAR_CODE -c pdvitype.c
pdvitype.c: pdvitype.p $(web2c_aux)
	$(web2c) pdvitype
pdvitype.p: tangle pdvitype.web $(srcdir)/ptexdir/pdvitype.ch
	$(tangle)  pdvitype.web $(srcdir)/ptexdir/pdvitype.ch
pdvitype.web: $(srcdir)/dvitype.web
	cat   $(srcdir)/dvitype.web > $@

### pBibTeX
pbibtex: pbibtex.o $(plib_o) $(kpathsea) $(ptexenc) $(proglib)
	$(kpathsea_link) pbibtex.o $(plib_o) $(ptexenc) $(LOADLIBES)
pbibtex.o: pbibtex.c $(srcdir)/ptexdir/kanji.h $(srcdir)/ptexdir/ptexhelp.h

pbibd.h:       $(srcdir)/ptexdir/pbibd.sed pbibtex.c
	sed -f $(srcdir)/ptexdir/pbibd.sed pbibtex.c > $@

pbibtex.c pbibtex.h: $(web2c_common) $(web2c_programs) web2c/cvtbib.sed pbibtex.p
	$(web2c) pbibtex
pbibtex.p: tangle bibtex.web $(srcdir)/ptexdir/pbibtex.ch
	$(tangle) bibtex.web $(srcdir)/ptexdir/pbibtex.ch
clean:: pbibtex-clean
pbibtex-clean:
	$(LIBTOOL) --mode=clean rm -f pbibtex
	rm -f pbibtex.o pbibtex.c pbibtex.h pbibtex.p pbibd.h


# Additional dependencies for relinking.
$(ptexenc):
	cd $(ptexenc_dir) && $(MAKE) libptexenc.la

# Tests...
check: @JPTEX@ ptex-check
ptex-check: etrip ptex.fmt
# Test truncation (but don't bother showing the warning msg).
	./ptex --progname=ptex --output-comment="`cat $(srcdir)/PROJECTS`" \
	  $(srcdir)/tests/hello 2>/dev/null \
	  && ./dvitype hello.dvi | grep olaf@infovore.xs4all.nl >/dev/null
# \openout should show up in \write's.
	./ptex --progname=ptex $(srcdir)/tests/openout && grep xfoo openout.log
# one.two.tex -> one.two.log
	./ptex --progname=ptex $(srcdir)/tests/one.two && ls -l one.two.log
# uno.dos -> uno.log
	./ptex --progname=ptex $(srcdir)/tests/uno.dos && ls -l uno.log
	./ptex --progname=ptex $(srcdir)/tests/just.texi && ls -l just.log
	-./ptex --progname=ptex $(srcdir)/tests/batch.tex
	./ptex --progname=ptex --shell $(srcdir)/tests/write18 | grep echo
# tcx files are a bad idea.
#	./ptex --translate-file=$(srcdir)/share/isol1-t1.tcx \
#	  $(srcdir)/tests/eight && ./dvitype eight.dvi >eigh.typ
	TEXMFCNF=../kpathsea \
	  ./ptex --mltex --progname=einitex $(srcdir)/tests/mltextst
	-./ptex --progname=ptex </dev/null
	-PATH=`pwd`:$(kpathsea_dir):$(kpathsea_srcdir):$$PATH \
	  WEB2C=$(kpathsea_srcdir) TMPDIR=.. \
	  ./ptex --progname=ptex '\nonstopmode\font\foo=nonesuch\end'

# Cleaning up.
clean:: ptex-clean
ptex-clean: etrip-clean
	$(LIBTOOL) --mode=clean $(RM) $(ptex)
	rm -f $(ptex_o) $(ptex_c) ptexextra.c ptexcoerce.h ptexd.h
	rm -f ptexdir/ptexextra.h ptexdir/ptex.version
	rm -f ptexdir/kanji.o $(plib_o) $(plib)
	rm -f ptex.p ptex.pool ptex.web ptex.ch ptex-pool.c
	rm -f ptftopl.h  ptftopl.c  ptftopl.o  ptftopl.web  ptftopl.p
	rm -f ppltotf.h  ppltotf.c  ppltotf.o  ppltotf.web  ppltotf.p
	rm -f pdvitype.h pdvitype.c pdvitype.o pdvitype.web pdvitype.p
	rm -f ptex.fmt ptex.log
	rm -f hello.dvi hello.log xfoo.out openout.log one.two.log uno.log
	rm -f just.log batch.log write18.log mltextst.log texput.log
	rm -f missfont.log
	rm -rf tfm

# etrip
ptestdir = $(srcdir)/ptexdir/etrip
ptestenv = TEXMFCNF=$(etestdir)

triptrap: @JPTEX@ etrip
ptrip: pltotf tftopl ptex dvitype etrip-clean
	@echo ">>> See $(etestdir)/etrip.diffs for example of acceptable diffs." >&2
	@echo "*** TRIP test for e-TeX in compatibility mode ***."
	./pltotf $(testdir)/trip.pl trip.tfm
	./tftopl ./trip.tfm trip.pl
	-diff $(testdir)/trip.pl trip.pl
	$(LN) $(testdir)/trip.tex . # get same filename in log
	-$(SHELL) -c '$(etestenv) ./ptex --progname=einitex --ini <$(testdir)/trip1.in >ctripin.fot'
	mv trip.log ctripin.log
	-diff $(testdir)/tripin.log ctripin.log
	-$(SHELL) -c '$(etestenv) ./ptex --progname=ptex <$(testdir)/trip2.in >ctrip.fot'
	mv trip.log ctrip.log
	-diff $(testdir)/trip.fot ctrip.fot
	-$(DIFF) $(DIFFFLAGS) $(testdir)/trip.log ctrip.log
	$(SHELL) -c '$(etestenv) ./dvitype $(dvitype_args) trip.dvi >ctrip.typ'
	-$(DIFF) $(DIFFFLAGS) $(testdir)/trip.typ ctrip.typ
	@echo "*** TRIP test for e-TeX in extended mode ***."
	-$(SHELL) -c '$(etestenv) ./ptex --progname=einitex --ini <$(etestdir)/etrip1.in >xtripin.fot'
	mv trip.log xtripin.log
	-diff ctripin.log xtripin.log
	-$(SHELL) -c '$(etestenv) ./ptex --progname=ptex <$(etestdir)/trip2.in >xtrip.fot'
	mv trip.log xtrip.log
	-diff ctrip.fot xtrip.fot
	-$(DIFF) $(DIFFFLAGS) ctrip.log xtrip.log
	$(SHELL) -c '$(etestenv) ./dvitype $(dvitype_args) trip.dvi >xtrip.typ'
	-$(DIFF) $(DIFFFLAGS) ctrip.typ xtrip.typ
	@echo "*** e-TeX specific part of e-TRIP test ***."
	./pltotf $(etestdir)/etrip.pl etrip.tfm
	./tftopl ./etrip.tfm etrip.pl
	-diff $(etestdir)/etrip.pl etrip.pl
	$(LN) $(etestdir)/etrip.tex . # get same filename in log
	-$(SHELL) -c '$(etestenv) ./ptex --progname=einitex --ini <$(etestdir)/etrip2.in >etripin.fot'
	mv etrip.log etripin.log
	-diff $(etestdir)/etripin.log etripin.log
	-$(SHELL) -c '$(etestenv) ./ptex --progname=ptex <$(etestdir)/etrip3.in >etrip.fot'
	-diff $(etestdir)/etrip.fot etrip.fot
	-$(DIFF) $(DIFFFLAGS) $(etestdir)/etrip.log etrip.log
	diff $(etestdir)/etrip.out etrip.out
	$(SHELL) -c '$(etestenv) ./dvitype $(dvitype_args) etrip.dvi >etrip.typ'
	-$(DIFF) $(DIFFFLAGS) $(etestdir)/etrip.typ etrip.typ

# Cleaning up for the etrip.
ptrip-clean:
	rm -f trip.tfm trip.pl trip.tex trip.fmt ctripin.fot ctripin.log
	rm -f ctrip.fot ctrip.log trip.dvi ctrip.typ
	rm -f xtripin.fot xtripin.log
	rm -f xtrip.fot xtrip.log xtrip.typ
	rm -f etrip.tfm etrip.pl etrip.tex etrip.fmt etripin.fot etripin.log
	rm -f etrip.fot etrip.log etrip.dvi etrip.out etrip.typ
	rm -f tripos.tex 8terminal.tex
	rm -rf tfm

# Distfiles ...
@MAINT@triptrapdiffs: ptexdir/etrip/etrip.diffs
@MAINT@ptexdir/etrip/etrip.diffs: ptex
@MAINT@	$(MAKE) ptrip | tail +1 >ptexdir/etrip/etrip.diffs


# Dumps
all_pfmts = ptex.fmt platex.fmt $(pfmts)

dumps: @JPTEX@ pfmts
pfmts: $(all_pfmts)

pfmtdir = $(web2cdir)/ptex
$(pfmtdir)::
	$(SHELL) $(top_srcdir)/../mkinstalldirs $(pfmtdir)

ptex.fmt: ptex
	$(dumpenv) $(MAKE) progname=ptex files="ptex.src plain.tex cmr10.tfm" prereq-check
	$(dumpenv) ./ptex --progname=ptex --jobname=ptex --ini \*\\input ptex.src \\dump </dev/null

#pelatex.fmt: ptex
#	$(dumpenv) $(MAKE) progname=elatex files="latex.ltx" prereq-check
#	$(dumpenv) ./ptex --progname=elatex --jobname=elatex --ini \*\\input latex.ltx </dev/null

platex.fmt: ptex
	$(dumpenv) $(MAKE) progname=platex files="platex.ltx" prereq-check
	$(dumpenv) ./ptex --progname=platex --jobname=platex --ini \*\\input latex.ltx </dev/null

#ctex.fmt: ptex
#	$(dumpenv) $(MAKE) progname=ctex files="plain.tex cmr10.tfm" prereq-check
#	$(dumpenv) ./ptex --progname=ctex --jobname=ctex --ini \\input plain \\dump </dev/null

#olatex.fmt: ptex
#	$(dumpenv) $(MAKE) progname=olatex files="latex.ltx" prereq-check
#	$(dumpenv) ./ptex --progname=olatex --progname=olatex --ini \\input latex.ltx </dev/null

# 
# Installation.
install-ptex: install-ptex-exec
install-programs: @JPTEX@ install-ptex-exec
install-ptex-exec: $(ptex) $(bindir)
	for p in $(ptex); do $(INSTALL_LIBTOOL_PROG) $$p $(bindir); done

install-fmts: @JPTEX@ install-ptex-fmts
install-ptex-fmts: pfmts $(pfmtdir)
	pfmts="$(all_pfmts)"; \
	  for f in $$pfmts; do $(INSTALL_DATA) $$f $(pfmtdir)/$$f; done
	pfmts="$(pfmts)"; \
	  for f in $$pfmts; do base=`basename $$f .fmt`; \
	    (cd $(bindir) && (rm -f $$base; $(LN) ptex $$base)); done

# end of ptex.mk
