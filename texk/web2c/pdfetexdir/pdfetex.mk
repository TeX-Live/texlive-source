# Makefile fragment for pdfeTeX and web2c. --infovore@xs4all.nl. Public domain.
# This fragment contains the parts of the makefile that are most likely to
# differ between releases of pdfeTeX.

Makefile: $(srcdir)/pdfetexdir/pdfetex.mk

# We build pdfetex
pdfetex = @PETEX@ pdfetex
pdfetexdir = pdfetexdir

# The C sources.
pdfetex_c = pdfetexini.c pdfetex0.c pdfetex1.c pdfetex2.c pdfetex3.c
pdfetex_o = pdfetexini.o pdfetex0.o pdfetex1.o pdfetex2.o pdfetex3.o pdfetexextra.o

# Making pdfetex
pdfetex: pdftexd.h $(pdfetex_o) $(pdfetexextra_o) $(pdftexlibsdep)
	@CXXHACKLINK@ $(pdfetex_o) $(pdfetexextra_o) $(pdftexlibs) $(socketlibs) @CXXHACKLDLIBS@ @CXXLDEXTRA@

# C file dependencies.
$(pdfetex_c) pdfetexcoerce.h pdfetexd.h: pdfetex.p $(web2c_texmf) $(srcdir)/pdfetexdir/pdfetex.defines
	$(web2c) pdfetex
pdfetexextra.c: pdfetexdir/pdfetexextra.h lib/texmfmp.c
	sed s/TEX-OR-MF-OR-MP/pdfetex/ $(srcdir)/lib/texmfmp.c >$@
pdfetexdir/pdfetexextra.h: pdfetexdir/pdfetexextra.in pdftexdir/pdftex.version etexdir/etex.version
	sed -e s/PDFTEX-VERSION/`cat pdftexdir/pdftex.version`/ \
	    -e s/ETEX-VERSION/`cat etexdir/etex.version`/ \
	  $(srcdir)/pdfetexdir/pdfetexextra.in >$@

# Tangling
pdfetex.p pdfetex.pool: tangle pdfetex.web pdfetex.ch
	$(TANGLE) pdfetex.web pdfetex.ch

# Generation of the web and ch file.
#   Sources for pdfetex.web:
pdfetex_web_srcs = $(srcdir)/tex.web \
  $(srcdir)/etexdir/etex.ch \
  $(srcdir)/etexdir/etex.fix \
  $(srcdir)/pdfetexdir/pdfetex.ch1 \
  $(srcdir)/pdftexdir/pdftex.ch \
  $(srcdir)/pdftexdir/hz.ch \
  $(srcdir)/pdftexdir/misc.ch \
  $(srcdir)/pdftexdir/vadjust.ch \
  $(srcdir)/pdftexdir/pdftex2.ch \
  $(srcdir)/pdfetexdir/pdfetex.ch2
#   Sources for pdfetex.ch:
pdfetex_ch_srcs = pdfetex.web \
  $(srcdir)/pdfetexdir/tex.ch0 \
  $(srcdir)/tex.ch \
  $(srcdir)/etexdir/tex.ch1 \
  $(srcdir)/etexdir/tex.ech \
  $(srcdir)/pdfetexdir/tex.ch1 \
  $(srcdir)/pdftexdir/tex.pch \
  $(srcdir)/pdftexdir/noligatures.ch \
  $(srcdir)/pdftexdir/pdfstrcmp.ch \
  $(srcdir)/pdftexdir/randoms.ch
#   Rules:
pdfetex.web: tie pdfetexdir/pdfetex.mk $(pdfetex_web_srcs)
	$(TIE) -m pdfetex.web $(pdfetex_web_srcs)
pdfetex.ch: $(pdfetex_ch_srcs)
	$(TIE) -c pdfetex.ch $(pdfetex_ch_srcs)

# for developing only
pdfetex-org.web: $(pdfetex_ch_srcs_org)
	$(TIE) -m $@ $(pdfetex_ch_srcs_org)
pdfetex-all.web: pdfetex.web pdfetex.ch
	$(TIE) -m $@ pdfetex.web pdfetex.ch

$(srcdir)/pdfetexdir/pdfetex.h: $(srcdir)/pdftexdir/pdftex.h
	cp -f $(srcdir)/pdftexdir/pdftex.h $@

$(srcdir)/pdfetexdir/pdfetex.defines: $(srcdir)/pdftexdir/pdftex.defines
	cp -f $(srcdir)/pdftexdir/pdftex.defines $@

check: @PETEX@ pdfetex-check
pdfetex-check: pdfetex pdfetex.fmt

clean:: pdfetex-clean
pdfetex-clean:
	$(LIBTOOL) --mode=clean $(RM) pdfetex
	rm -f $(pdfetex_o) $(pdfetex_c) pdfetexextra.c pdfetexcoerce.h
	rm -f pdfetexdir/pdfetexextra.h
	rm -f pdfetexd.h pdfetex.p pdfetex.pool pdfetex.web pdfetex.ch
	rm -f pdfetex.fmt pdfetex.log

# Dumps
all_pdfefmts = @FMU@ pdfetex.fmt $(pdfefmts)

dumps: @PETEX@ pdfefmts
pdfefmts: $(all_pdfefmts)

pdfefmtdir = $(web2cdir)/pdfetex
$(pdfefmtdir)::
	$(SHELL) $(top_srcdir)/../mkinstalldirs $(pdfefmtdir)

pdfetex.fmt: pdfetex
	$(dumpenv) $(MAKE) progname=pdfetex files="etex.src plain.tex cmr10.tfm" prereq-check
	$(dumpenv) ./pdfetex --progname=pdfetex --jobname=pdfetex --ini \*\\pdfoutput=1\\input etex.src \\dump </dev/null

pdflatex.fmt: pdfetex
	$(dumpenv) $(MAKE) progname=pdflatex files="latex.ltx" prereq-check
	$(dumpenv) ./pdfetex --progname=pdflatex --jobname=pdflatex --ini \*\\pdfoutput=1\\input latex.ltx </dev/null

# 
# Installation.
install-pdfetex: install-pdfetex-exec install-pdfetex-data
install-pdfetex-exec: install-pdfetex-links
install-pdfetex-data: install-pdfetex-pool @FMU@ install-pdfetex-dumps
install-pdfetex-dumps: install-pdfetex-fmts

# The actual binary executables and pool files.
install-programs: @PETEX@ install-pdfetex-programs
install-pdfetex-programs: $(pdfetex) $(bindir)
	for p in pdfetex; do $(INSTALL_LIBTOOL_PROG) $$p $(bindir); done

install-links: @PETEX@ install-pdfetex-links
install-pdfetex-links: install-pdfetex-programs
	#cd $(bindir) && (rm -f pdfeinitex pdfevirtex; \
	#  $(LN) pdfetex pdfeinitex; $(LN) pdfetex pdfevirtex)

install-fmts: @PETEX@ install-pdfetex-fmts
install-pdfetex-fmts: pdfefmts $(pdfefmtdir)
	pdfefmts="$(all_pdfefmts)"; \
	  for f in $$pdfefmts; do $(INSTALL_DATA) $$f $(pdfefmtdir)/$$f; done
	pdfefmts="$(pdfefmts)"; \
	  for f in $$pdfefmts; do base=`basename $$f .fmt`; \
	    (cd $(bindir) && (rm -f $$base; $(LN) pdfetex $$base)); done

# Auxiliary files.
install-data:: @PETEX@ install-pdfetex-data
install-pdfetex-pool: pdfetex.pool $(texpooldir)
	$(INSTALL_DATA) pdfetex.pool $(texpooldir)/pdfetex.pool

# end of pdfetex.mk
