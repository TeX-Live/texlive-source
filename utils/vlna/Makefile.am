# Makefile.am -- Peter Breitenlohner <peb@mppmu.mpg.de> -- Public domain.

bin_PROGRAMS = vlna
dist_man1_MANS = vlna.man

CWEB_SRC = vlna.w
CWEB_TEX = vlna.tex csmac.tex
CWEB_DVI = vlna.dvi

DOCS = README README.en vlna.txt

EXTRA_DIST = $(CWEB_SRC) $(CWEB_TEX) $(CWEB_DVI) $(DOCS)

TMP_FILES = vlna.idx vlna.scn

if MAINTAINER_MODE
all-local: vlna.dvi
vlna.c: vlna.w
	$(CTANGLE) $<
vlna.tex: vlna.w
	$(CWEAVE) $< && rm -f $(TMP_FILES)
vlna.dvi: vlna.tex
	TEXINPUTS=.:$(srcdir):$$TEXINPUTS $(CSPLAIN) vlna.tex
else
all-local:
endif
