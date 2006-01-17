# Makefile fragment for pdfTeX and web2c. --infovore@xs4all.nl. Public domain.
# This fragment contains the parts of the makefile that are most likely to
# differ between releases of pdfTeX.

# The libraries are not mentioned.  As the matter stands, a change in their
# number or how they are configured requires changes to the main distribution
# anyway.

# $Id: //depot/Build/source.development/TeX/texk/web2c/pdftexdir/pdftex.mk#38 $

Makefile: pdftexdir/pdftex.mk

pdftex_bin = pdftex pdfetex ttf2afm pdftosrc
linux_build_dir = $(HOME)/pdftex/build/linux/texk/web2c

# We build pdftex
pdftex = @PTEX@ pdftex
pdftexdir = pdftexdir

# Extract pdftex version
pdftexdir/pdftex.version: pdftexdir/pdftex.ch
	grep '^@d pdftex_version_string==' $(srcdir)/pdftexdir/pdftex.ch \
	  | sed "s/^.*'-//;s/'.*$$//" \
	  >pdftexdir/pdftex.version

# The C sources.
pdftex_c = pdftexini.c pdftex0.c pdftex1.c pdftex2.c pdftex3.c
pdftex_o = pdftexini.o pdftex0.o pdftex1.o pdftex2.o pdftex3.o pdftexextra.o

# Making pdftex
pdftex: $(pdftex_o) $(pdftexextra_o) $(pdftexlibsdep)
	@CXXHACKLINK@ $(pdftex_o) $(pdftexextra_o) $(pdftexlibs) $(socketlibs) @CXXHACKLDLIBS@ @CXXLDEXTRA@

# C file dependencies.
$(pdftex_c) pdftexcoerce.h pdftexd.h: pdftex.p $(web2c_texmf)
	$(web2c) pdftex
pdftexextra.c: pdftexdir/pdftexextra.h lib/texmfmp.c
	sed s/TEX-OR-MF-OR-MP/pdftex/ $(srcdir)/lib/texmfmp.c >$@
pdftexdir/pdftexextra.h: pdftexdir/pdftexextra.in pdftexdir/pdftex.version
	sed s/PDFTEX-VERSION/`cat pdftexdir/pdftex.version`/ \
	  $(srcdir)/pdftexdir/pdftexextra.in >$@

# Tangling.
pdftex.p pdftex.pool: tangle pdftex.web pdftex.ch
	$(TANGLE) pdftex.web pdftex.ch

# Generation of the web and ch files.
#   Sources for pdftex.web:
pdftex_web_srcs = $(srcdir)/tex.web \
  $(srcdir)/pdftexdir/pdftex.ch \
  $(srcdir)/pdftexdir/hz.ch  \
  $(srcdir)/pdftexdir/misc.ch \
  $(srcdir)/pdftexdir/vadjust.ch \
  $(srcdir)/pdftexdir/pdftex2.ch
#   Sources for etex.ch:
pdftex_ch_srcs = pdftex.web \
  $(srcdir)/pdftexdir/tex.ch0 \
  $(srcdir)/tex.ch \
  $(srcdir)/pdftexdir/tex.ch1 \
  $(srcdir)/pdftexdir/tex.pch \
  $(srcdir)/pdftexdir/noligatures.ch \
  $(srcdir)/pdftexdir/pdfstrcmp.ch \
  $(srcdir)/pdftexdir/randoms.ch
#   Rules:
pdftex.web: tie pdftexdir/pdftex.mk $(pdftex_web_srcs)
	$(TIE) -m pdftex.web $(pdftex_web_srcs)
pdftex.ch: $(pdftex_ch_srcs)
	$(TIE) -c pdftex.ch $(pdftex_ch_srcs)

# for developing only
pdftex-org.web: $(pdftex_ch_srcs_org)
	$(TIE) -m $@ $(pdftex_ch_srcs_org)
pdftex-all.web: pdftex.web pdftex.ch
	$(TIE) -m $@ pdftex.web pdftex.ch

# Tests...
check: @PTEX@ pdftex-check
pdftex-check: pdftex pdftex.fmt

# Cleaning up.
clean:: pdftex-clean
pdftex-clean:
	$(LIBTOOL) --mode=clean $(RM) pdftex
	rm -f $(pdftex_o) $(pdftex_c) pdftexextra.c pdftexcoerce.h
	rm -f pdftexdir/pdftexextra.h pdftexdir/pdftex.version
	rm -f pdftexd.h pdftex.p pdftex.pool pdftex.web pdftex.ch
	rm -f pdftex.fmt pdftex.log

# Dumps.
all_pdffmts = @FMU@ pdftex.fmt $(pdffmts)

dumps: @PTEX@ pdffmts
pdffmts: $(all_pdffmts)

pdffmtdir = $(web2cdir)/pdftex
$(pdffmtdir)::
	$(SHELL) $(top_srcdir)/../mkinstalldirs $(pdffmtdir)

pdftex.fmt: pdftex
	$(dumpenv) $(MAKE) progname=pdftex files="plain.tex cmr10.tfm" prereq-check
	$(dumpenv) ./pdftex --progname=pdftex --jobname=pdftex --ini \\pdfoutput=1 \\input plain \\dump </dev/null

pdfolatex.fmt: pdftex
	$(dumpenv) $(MAKE) progname=pdfolatex files="latex.ltx" prereq-check
	$(dumpenv) ./pdftex --progname=pdfolatex --jobname=pdfolatex --ini \\pdfoutput=1 \\input latex.ltx </dev/null

#pdflatex.fmt: pdftex
#	$(dumpenv) $(MAKE) progname=pdflatex files="latex.ltx" prereq-check
#	$(dumpenv) ./pdftex --progname=pdflatex --jobname=pdflatex --ini \\pdfoutput=1 \\input latex.ltx </dev/null

# 
# Installation.
install-pdftex: install-pdftex-exec install-pdftex-data
install-pdftex-exec: install-pdftex-links
install-pdftex-data: install-pdftex-pool @FMU@ install-pdftex-dumps
install-pdftex-dumps: install-pdftex-fmts

# The actual binary executables and pool files.
install-programs: @PTEX@ install-pdftex-programs
install-pdftex-programs: pdftex $(bindir)
	for p in pdftex; do $(INSTALL_LIBTOOL_PROG) $$p $(bindir); done

install-links: @PTEX@ install-pdftex-links
install-pdftex-links: install-pdftex-programs
	#cd $(bindir) && (rm -f pdfinitex pdfinitex; \
	#  $(LN) pdftex pdfinitex; $(LN) pdftex pdfvirtex)

install-fmts: @PTEX@ install-pdftex-fmts
install-pdftex-fmts: pdffmts $(pdffmtdir)
	pdffmts="$(all_pdffmts)"; \
	  for f in $$pdffmts; do $(INSTALL_DATA) $$f $(pdffmtdir)/$$f; done
	pdffmts="$(pdffmts)"; \
	  for f in $$pdffmts; do base=`basename $$f .fmt`; \
	    (cd $(bindir) && (rm -f $$base; $(LN) pdftex $$base)); done

# Auxiliary files.
install-data:: @PTEX@ install-pdftex-data
install-pdftex-pool: pdftex.pool $(texpooldir)
	$(INSTALL_DATA) pdftex.pool $(texpooldir)/pdftex.pool


# 
# ttf2afm
ttf2afm = ttf2afm

ttf2afm: ttf2afm.o
	$(kpathsea_link) ttf2afm.o $(kpathsea)
ttf2afm.o: ttf2afm.c macnames.c
	$(compile) -c $< -o $@
ttf2afm.c:
	cp $(srcdir)/pdftexdir/ttf2afm.c .
macnames.c:
	cp $(srcdir)/pdftexdir/macnames.c .
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

pdftosrc: pdftexdir/pdftosrc.o $(LIBXPDFDEP)
	@CXXHACKLINK@ pdftexdir/pdftosrc.o $(LDLIBXPDF) -lm @CXXLDEXTRA@
pdftexdir/pdftosrc.o:$(srcdir)/pdftexdir/pdftosrc.cc
	cd pdftexdir && $(MAKE) pdftosrc.o
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
	$(MAKE) web2c-cross
	$(MAKE) pdftexbin

web2c-cross: $(web2c_programs)
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

# end of pdftex.mk
