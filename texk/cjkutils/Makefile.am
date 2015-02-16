## Makefile.am for the TeX Live subdirectory texk/cjkutils/
##
## Copyright (C) 2009-2015 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
## We want to re-distribute the whole original cjk/utils source tree.
EXTRA_DIST = $(CJKUTILS_TREE)
## Patches applied to the original source tree
EXTRA_DIST += $(CJKUTILS_TREE)-PATCHES

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

if do_cweb
all-local: config.force hbf2gf.dvi
else !do_cweb
all-local: config.force
endif !do_cweb

config.force: $(KPATHSEA_DEPEND)
	echo timestamp >config.force
	$(SHELL) ./config.status --recheck

DISTCLEANFILES = config.force

AM_CPPFLAGS = $(KPATHSEA_INCLUDES) $(XX_CPPFLAGS)
AM_CFLAGS = $(XX_CFLAGS)

bin_PROGRAMS = \
	bg5conv \
	cefconv \
	cef5conv \
	cefsconv \
	extconv \
	sjisconv \
	hbf2gf

hbf2gf_SOURCES = \
	@CJKUTILS_TREE@/hbf2gf/hbf.c \
	@CJKUTILS_TREE@/hbf2gf/hbf.h \
	hbf2gf.c
hbf2gf_CPPFLAGS = $(AM_CPPFLAGS) -I$(srcdir)/$(CJKUTILS_TREE)/hbf2gf -Dunix -DHAVE_LIBKPATHSEA
hbf2gf_DEPENDENCIES = $(KPATHSEA_DEPEND)
hbf2gf_LDADD = $(KPATHSEA_LIBS)

if WIN32
bin_PROGRAMS += \
	gbklatex \
	gbkpdflatex
endif WIN32

## For WIN32 we compile gbk{,pdf}latex.exe from Fabrice's cjklatex.c
## and copy them to{bg5,cef5,cef,cefs,sjis}{,pdf}latex.exe.
##
## For Unix systems we install the {bg5,cef5,cef,cefs,gbk,sjis}{,pdf}latex
## shell scripts and create symlinks bg5+{,pdf}latex -> gbk{,pdf}latex.
##
if WIN32
HOOK_DST = bg5 cef5 cef cefs sjis
else !WIN32
bin_links = gbklatex:bg5+latex gbkpdflatex:bg5+pdflatex
endif !WIN32

include $(srcdir)/../../am/bin_links.am

gbklatex_SOURCES = \
	@CJKUTILS_TREE@/cjklatex/cjklatex.c
gbklatex_DEPENDENCIES = $(KPATHSEA_DEPEND)
gbklatex_LDADD = $(KPATHSEA_LIBS)

gbkpdflatex_SOURCES = $(gbklatex_SOURCES)
gbkpdflatex_CPPFLAGS = $(AM_CPPFLAGS) -DLATEX=\"pdflatex\"
gbkpdflatex_DEPENDENCIES = $(gbklatex_DEPENDENCIES)
gbkpdflatex_LDADD = $(gbklatex_LDADD)

## Rebuild libkpathsea
@KPATHSEA_RULE@

if WIN32
bin_SCRIPTS =
install-exec-hook:
	@cd $(DESTDIR)$(bindir) && \
	  for p in latex pdflatex; do \
	    for c in $(HOOK_DST); do \
	      echo "cp gbk$$p$(EXEEXT) $$c$$p$(EXEEXT)"; \
	      rm -f $$c$$p$(EXEEXT); cp gbk$$p$(EXEEXT) $$c$$p$(EXEEXT); \
	    done; \
	  done
uninstall-hook:
	@for p in latex pdflatex; do \
	  for c in $(HOOK_DST); do \
	    echo "rm -f $(DESTDIR)$(bindir)/$$c$$p$(EXEEXT)"; \
	    rm -f $(DESTDIR)$(bindir)/$$c$$p$(EXEEXT); \
	  done; \
	done
else !WIN32
bin_SCRIPTS = \
	$(CJKUTILS_TREE)/Bg5conv/bg5latex \
	$(CJKUTILS_TREE)/Bg5conv/bg5pdflatex \
	$(CJKUTILS_TREE)/CEFconv/cef5latex \
	$(CJKUTILS_TREE)/CEFconv/cef5pdflatex \
	$(CJKUTILS_TREE)/CEFconv/ceflatex \
	$(CJKUTILS_TREE)/CEFconv/cefpdflatex \
	$(CJKUTILS_TREE)/CEFconv/cefslatex \
	$(CJKUTILS_TREE)/CEFconv/cefspdflatex \
	$(CJKUTILS_TREE)/SJISconv/sjislatex \
	$(CJKUTILS_TREE)/SJISconv/sjispdflatex \
	$(CJKUTILS_TREE)/extconv/gbklatex \
	$(CJKUTILS_TREE)/extconv/gbkpdflatex
install-exec-hook: install-bin-links
uninstall-hook: uninstall-bin-links
endif !WIN32

man1_MANS = \
	$(CJKUTILS_TREE)/Bg5conv/bg5conv.1 \
	$(CJKUTILS_TREE)/CEFconv/cef5conv.1 \
	$(CJKUTILS_TREE)/CEFconv/cefconv.1 \
	$(CJKUTILS_TREE)/CEFconv/cefsconv.1 \
	$(CJKUTILS_TREE)/SJISconv/sjisconv.1 \
	$(CJKUTILS_TREE)/extconv/extconv.1 \
	$(CJKUTILS_TREE)/hbf2gf/hbf2gf.1

hbf2gf_cfgdir = $(datarootdir)/texmf-dist/hbf2gf
CFG = $(CJKUTILS_TREE)/hbf2gf/cfg
hbf2gf_cfg_DATA = \
	$(CFG)/b5ka12.cfg \
	$(CFG)/b5kr12.cfg \
	$(CFG)/b5so12.cfg \
	$(CFG)/c1so12.cfg \
	$(CFG)/c2so12.cfg \
	$(CFG)/c3so12.cfg \
	$(CFG)/c4so12.cfg \
	$(CFG)/c5so12.cfg \
	$(CFG)/c6so12.cfg \
	$(CFG)/c7so12.cfg \
	$(CFG)/csso12.cfg \
	$(CFG)/gsfs14.cfg \
	$(CFG)/j2so12.cfg \
	$(CFG)/jsso12.cfg \
	$(CFG)/ksso17.cfg

CWEB_SRC = $(CJKUTILS_TREE)/hbf2gf/hbf2gf.w
CWEB_TEX = hbf2gf.tex hbf2gf.idx hbf2gf.scn
CWEB_DVI = hbf2gf.dvi

EXTRA_DIST += $(CWEB_TEX) $(CWEB_DVI)

CLEANFILES = hbf2gf.log hbf2gf.toc

hbf2gf.dvi: $(CWEB_TEX)
	TEXINPUTS=.:$(srcdir):$$TEXINPUTS $(TEX) hbf2gf.tex

# Running cweave in order to create hbf2gf.tex generates in addition
# hbf2gf.idx and hbf2gf.scn; we must rerun cweave when they are not
# up to date (even if hbf2gf.tex is up to date). Thus the sub-make.
#
hbf2gf.tex hbf2gf.idx hbf2gf.scn: $(CWEB_SRC)
	$(MAKE) $(AM_MAKEFLAGS) do-cweave
do-cweave: $(CWEB_SRC) .FORCE
	$(CWEAVE) +ai $<
.PHONY: do-cweave .FORCE

if do_cweb
bg5conv.c: $(CJKUTILS_TREE)/Bg5conv/bg5conv.w
	$(CTANGLE) $<
cef5conv.c: $(CJKUTILS_TREE)/CEFconv/cef5conv.w
	$(CTANGLE) $<
cefconv.c: $(CJKUTILS_TREE)/CEFconv/cefconv.w
	$(CTANGLE) $<
cefsconv.c: $(CJKUTILS_TREE)/CEFconv/cefsconv.w
	$(CTANGLE) $<
sjisconv.c: $(CJKUTILS_TREE)/SJISconv/sjisconv.w
	$(CTANGLE) $<
extconv.c: $(CJKUTILS_TREE)/extconv/extconv.w
	$(CTANGLE) $<
hbf2gf.c: $(CWEB_SRC)
	$(CTANGLE) $<
endif do_cweb

## Tests
##
TESTS = bg5conv.test cefconv.test cef5conv.test cefsconv.test extconv.test sjisconv.test
EXTRA_DIST += $(TESTS)

## bg5conv.test
EXTRA_DIST += tests/bg5conv.tex tests/bg5conv.cjk
DISTCLEANFILES += bg5conv.cjk
## cefconv.test
EXTRA_DIST += tests/cefconv.tex tests/cefconv.cjk
DISTCLEANFILES += cefconv.cjk
## cef5conv.test
EXTRA_DIST += tests/cef5conv.tex tests/cef5conv.cjk
DISTCLEANFILES += cef5conv.cjk
## cefsconv.test
EXTRA_DIST += tests/cefsconv.tex tests/cefsconv.cjk
DISTCLEANFILES += cefsconv.cjk
## extconv.test
EXTRA_DIST += tests/extconv.tex tests/extconv.cjk
DISTCLEANFILES += extconv.cjk
## sjisconv.test
EXTRA_DIST += tests/sjisconv.tex tests/sjisconv.cjk
DISTCLEANFILES += sjisconv.cjk

