# Makefile.am -- Peter Breitenlohner <peb@mppmu.mpg.de> -- Public domain.

bin_PROGRAMS = vlna

MAN_DEP =
if MAINTAINER_MODE
# Ensure vlna.dvi is up to date
MAN_DEP += $(CWEB_DVI)
endif

nodist_man1_MANS = vlna.1

MAN_SRC = vlna.man

DOCS = Makefile.orig README README.en vlna.txt

CWEB_SRC = vlna.w

CWEB_TEX = vlna.tex csmac.tex

CWEB_DVI = vlna.dvi

CWEB_EXE = vlna.exe

EXTRA_DIST = $(DOCS) $(MAN_SRC) $(CWEB_SRC) $(CWEB_TEX) $(CWEB_DVI) $(CWEB_EXE)

TMP_FILES = vlna.idx vlna.scn

if TETEX_BUILD
BIN_DIR = BINDIR
MAN_DIR = MANDIR/man1
else
BIN_DIR = $(bindir)
MAN_DIR = $(man1dir)
endif

vlna.1: vlna.man $(MAN_DEP)
	sed -e "s|@BINDIR@|$(BIN_DIR)|;\
		s|@MANDIR@|$(MAN_DIR)|;\
		s|@MANEXT@|1|" $< > $@

if MAINTAINER_MODE

vlna.c: vlna.w
	$(CTANGLE) $<

vlna.tex: vlna.w
	$(CWEAVE) $< && rm -f $(TMP_FILES)

vlna.dvi: vlna.tex
	TEXINPUTS=.:$(srcdir):$$TEXINPUTS $(CSPLAIN) vlna.tex

endif
