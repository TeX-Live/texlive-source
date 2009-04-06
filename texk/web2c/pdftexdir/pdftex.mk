# Makefile fragment for pdfeTeX and web2c. --infovore@xs4all.nl. Public domain.
# This fragment contains the parts of the makefile that are most likely to
# differ between releases of pdfeTeX.

# We build pdftex
pdftex = @PETEX@ pdftex
pdftexdir = pdftexdir

LIBOBSDCOMPATDIR=../../libs/obsdcompat
LIBOBSDCOMPATSRCDIR=$(srcdir)/$(LIBOBSDCOMPATDIR)
XCPPFLAGS=-I$(LIBOBSDCOMPATDIR) -I$(LIBOBSDCOMPATDIR)/.. -I$(LIBOBSDCOMPATSRCDIR) -I$(LIBOBSDCOMPATSRCDIR)/.. -I$(ZLIBSRCDIR)

Makefile: $(srcdir)/$(pdftexdir)/pdftex.mk

pdftex_bin = pdftex ttf2afm pdftosrc
linux_build_dir = $(HOME)/pdftex/build/linux/texk/web2c

# Extract pdftex version
$(pdftexdir)/pdftex.version: $(srcdir)/$(pdftexdir)/pdftex.web
	test -d $(pdftexdir) || mkdir $(pdftexdir)
	grep '^@d pdftex_version_string==' $(srcdir)/$(pdftexdir)/pdftex.web \
	  | sed "s/^.*'-//;s/'.*$$//" \
	  >$(pdftexdir)/pdftex.version

# The C sources.
pdftex_c = pdftexini.c pdftex0.c pdftex1.c pdftex2.c pdftex3.c
pdftex_o = pdftexini.o pdftex0.o pdftex1.o pdftex2.o pdftex3.o pdftexextra.o \
pdftex-pool.o $(pdftex_o-with_synctex)

# Making pdftex
pdftex: pdftexd.h $(pdftex_o) $(pdftexextra_o) $(pdftexlibsdep)
	@CXXHACKLINK@ $(pdftex_o) $(pdftexextra_o) $(pdftexlibs) $(socketlibs) @CXXHACKLDLIBS@ @CXXLDEXTRA@

# C file dependencies.
$(pdftex_c) pdftexcoerce.h pdftexd.h: pdftex.p $(web2c_texmf) $(srcdir)/$(pdftexdir)/pdftex.defines $(srcdir)/$(pdftexdir)/pdftex.h
	$(web2c) pdftex
	$(pdftexd.h-with_synctex)
pdftexextra.c: pdftexd.h $(pdftexdir)/pdftexextra.h lib/texmfmp.c
	test -d $(pdftexdir) || mkdir $(pdftexdir)
	sed s/TEX-OR-MF-OR-MP/pdftex/ $(srcdir)/lib/texmfmp.c >$@
$(pdftexdir)/pdftexextra.h: $(pdftexdir)/pdftexextra.in $(pdftexdir)/pdftex.version etexdir/etex.version
	test -d $(pdftexdir) || mkdir $(pdftexdir)
	sed -e s/PDFTEX-VERSION/`cat $(pdftexdir)/pdftex.version`/ \
	    -e s/ETEX-VERSION/`cat etexdir/etex.version`/ \
	  $(srcdir)/$(pdftexdir)/pdftexextra.in >$@
pdftex-pool.c: pdftex.pool $(makecpool)
	$(makecpool) pdftex.pool pdftexdir/ptexlib.h >$@ || rm -f $@

# Tangling
pdftex.p pdftex.pool: tangle $(srcdir)/$(pdftexdir)/pdftex.web pdftex.ch
	$(TANGLE) $(srcdir)/$(pdftexdir)/pdftex.web pdftex.ch

#   Sources for pdftex.ch:
pdftex_ch_srcs = $(srcdir)/$(pdftexdir)/pdftex.web \
  $(srcdir)/$(pdftexdir)/tex.ch0 \
  $(srcdir)/tex.ch \
  $(pdftex_ch_srcs-with_synctex) \
  $(srcdir)/$(pdftexdir)/pdftex.ch
#   Rules:
pdftex.ch: $(TIE) $(pdftex_ch_srcs)
	$(TIE) -c pdftex.ch $(pdftex_ch_srcs)

# pdfetex: (it's just a copy of pdftex)
pdfetex: pdftex
	cp -p pdftex pdfetex
pdfetex.pool: pdftex.pool
	cp -p pdftex.pool pdfetex.pool

# for developing only
pdftex-org.web: $(TIE) $(pdftex_ch_srcs_org)
	$(TIE) -m $@ $(pdftex_ch_srcs_org)
pdftex-all.web: $(TIE) $(srcdir)/$(pdftexdir)/pdftex.web pdftex.ch
	$(TIE) -m $@ $(srcdir)/$(pdftexdir)/pdftex.web pdftex.ch
pdftex-all.tex: pdftex-all.web
	$(WEAVE) pdftex-all.web
	{ echo '1s/ webmac/ pdfwebmac/'; echo w; echo q;} | ed $@ >/dev/null 2>&1
pdftex-all.pdf: pdftex-all.tex
	$(pdftex) pdftex-all.tex

check: @PETEX@ pdftex-check
pdftex-check: pdftex pdftex.fmt

clean:: pdftex-clean
pdftex-clean:
	$(LIBTOOL) --mode=clean $(RM) pdftex
	rm -f $(pdftex_o) $(pdftex_c) pdftexextra.c pdftexcoerce.h
	rm -f $(pdftexdir)/pdftexextra.h
	rm -f pdftexd.h pdftex.p pdftex.pool pdftex.ch
	rm -f pdftex.fmt pdftex.log

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

pdflatex.fmt: pdftex
	$(dumpenv) $(MAKE) progname=pdflatex files="latex.ltx" prereq-check
	$(dumpenv) ./pdftex --progname=pdflatex --jobname=pdflatex --ini \*\\pdfoutput=1\\input latex.ltx </dev/null

# 
# Installation.
install-pdftex: install-pdftex-exec
install-programs: @PETEX@ install-pdftex-exec
install-pdftex-exec: $(pdftex) $(bindir)
	for p in pdftex; do $(INSTALL_LIBTOOL_PROG) $$p $(bindir); done

# 
# ttf2afm
ttf2afm = ttf2afm

ttf2afm: ttf2afm.o
	$(kpathsea_link) ttf2afm.o $(kpathsea)
ttf2afm.o: ttf2afm.c macnames.c
	$(compile) -c $< -o $@
ttf2afm.c: $(srcdir)/$(pdftexdir)/ttf2afm.c
	cp $(srcdir)/$(pdftexdir)/ttf2afm.c .
macnames.c: $(srcdir)/$(pdftexdir)/macnames.c
	cp $(srcdir)/$(pdftexdir)/macnames.c .
check: ttf2afm-check
ttf2afm-check: ttf2afm
clean:: ttf2afm-clean
ttf2afm-clean:
	$(LIBTOOL) --mode=clean $(RM) ttf2afm
	rm -f ttf2afm.o macnames.o
	rm -f ttf2afm.c macnames.c
# 
# pdftosrc
pdftosrc = pdftosrc

pdftosrc: $(pdftexdir)/pdftosrc.o $(LIBXPDFDEP)
	@CXXHACKLINK@ $(pdftexdir)/pdftosrc.o $(LDLIBXPDF) -lm @CXXLDEXTRA@
$(pdftexdir)/pdftosrc.o:$(srcdir)/$(pdftexdir)/pdftosrc.cc
	cd $(pdftexdir) && $(MAKE) pdftosrc.o
check: pdftosrc-check
pdftosrc-check: pdftosrc
clean:: pdftosrc-clean
pdftosrc-clean:
	$(LIBTOOL) --mode=clean $(RM) pdftosrc
# 
# pdftex binaries archive
pdftexbin:
	$(MAKE) $(pdftex_bin)

pdftex-cross:
	$(MAKE) pdftex-web2c-cross
	$(MAKE) pdftexbin

pdftex-web2c-cross: $(web2c_programs)
	@if test ! -x $(linux_build_dir)/tangle; then echo Error: linux_build_dir not ready; exit -1; fi
	rm -f web2c/fixwrites web2c/splitup web2c/web2c
	cp -f $(linux_build_dir)/web2c/fixwrites web2c
	cp -f $(linux_build_dir)/web2c/splitup web2c
	cp -f $(linux_build_dir)/web2c/web2c web2c
	touch web2c/fixwrites web2c/splitup web2c/web2c
	$(MAKE) tangleboot && rm -f tangleboot && \
	cp -f $(linux_build_dir)/tangleboot .  && touch tangleboot
	$(MAKE) ctangleboot && rm -f ctangleboot && \
	cp -f $(linux_build_dir)/ctangleboot .  && touch ctangleboot
	$(MAKE) ctangle && rm -f ctangle && \
	cp -f $(linux_build_dir)/ctangle .  && touch ctangle
	$(MAKE) tie && rm -f tie && \
	cp -f $(linux_build_dir)/tie .  && touch tie
	$(MAKE) tangle && rm -f tangle && \
	cp -f $(linux_build_dir)/tangle .  && touch tangle

# vim: set noexpandtab
# end of pdftex.mk
