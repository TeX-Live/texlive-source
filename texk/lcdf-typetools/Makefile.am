## Process this file with automake to produce Makefile.in
ACLOCAL_AMFLAGS = -I ../../m4 -I m4

AM_DISTCHECK_CONFIGURE_FLAGS = --without-kpathsea

SUBDIRS = . liblcdf libefont @SELECTED_SUBDIRS@
DIST_SUBDIRS = liblcdf libefont cfftot1 mmafm mmpfb otfinfo otftotfm t1dotlessj t1lint t1rawafm t1reencode t1testpage ttftotype42

all-local: config.force

config.force: $(KPATHSEA_DEPEND)
	echo timestamp >config.force
	$(SHELL) ./config.status --recheck

DISTCLEANFILES = config.force CXXLD.sh

if texlive_build
glyphlistdir = $(datarootdir)/texmf-dist/fonts/map/glyphlist
encdir = $(datarootdir)/texmf-dist/fonts/enc/dvips/base
else !texlive_build
glyphlistdir = $(pkgdatadir)
encdir = $(pkgdatadir)
endif !texlive_build

glyphlist_DATA = glyphlist.txt texglyphlist.txt
enc_DATA = 7t.enc

EXTRA_DIST = \
	ONEWS \
	lcdf-typetools.spec \
	include/config.h \
	include/lcdf/bezier.hh \
	include/lcdf/clp.h \
	include/lcdf/error.hh \
	include/lcdf/filename.hh \
	include/lcdf/globmatch.hh \
	include/lcdf/hashcode.hh \
	include/lcdf/hashmap.hh include/lcdf/hashmap.cc \
	include/lcdf/inttypes.h \
	include/lcdf/landmark.hh \
	include/lcdf/md5.h \
	include/lcdf/permstr.hh \
	include/lcdf/point.hh \
	include/lcdf/slurper.hh \
	include/lcdf/straccum.hh \
	include/lcdf/string.hh \
	include/lcdf/strtonum.h \
	include/lcdf/transform.hh \
	include/lcdf/vector.hh include/lcdf/vector.cc \
	include/efont/afm.hh \
	include/efont/afmparse.hh \
	include/efont/afmw.hh \
	include/efont/amfm.hh \
	include/efont/cff.hh \
	include/efont/encoding.hh \
	include/efont/findmet.hh \
	include/efont/metrics.hh \
	include/efont/otf.hh \
	include/efont/otfcmap.hh \
	include/efont/otfdata.hh \
	include/efont/otfgpos.hh \
	include/efont/otfgsub.hh \
	include/efont/otfname.hh \
	include/efont/otfos2.hh \
	include/efont/otfpost.hh \
	include/efont/pairop.hh \
	include/efont/psres.hh \
	include/efont/t1bounds.hh \
	include/efont/t1cs.hh \
	include/efont/t1csgen.hh \
	include/efont/t1font.hh \
	include/efont/t1interp.hh \
	include/efont/t1item.hh \
	include/efont/t1mm.hh \
	include/efont/t1rw.hh \
	include/efont/t1unparser.hh \
	include/efont/ttfcs.hh \
	include/efont/ttfhead.hh \
	include/efont/ttfkern.hh \
	glyphlist.txt \
	texglyphlist.txt \
	texglyphlist-g2u.txt \
	7t.enc

liblcdf libefont:
	cd $@ && $(MAKE)
libefont: liblcdf
cfftot1 mmafm mmpfb otfinfo otftotfm t1dotlessj t1lint t1rawafm t1reencode t1testpage ttftotype42: liblcdf libefont
	cd $@ && $(MAKE)

versionize:
	perl -pi -e 's/^\.ds V.*/.ds V $(VERSION)/;' $(top_srcdir)/cfftot1/cfftot1.1 $(top_srcdir)/mmafm/mmafm.1 $(top_srcdir)/mmpfb/mmpfb.1 $(top_srcdir)/otfinfo/otfinfo.1 $(top_srcdir)/otftotfm/otftotfm.1 $(top_srcdir)/t1dotlessj/t1dotlessj.1 $(top_srcdir)/t1lint/t1lint.1 $(top_srcdir)/t1rawafm/t1rawafm.1 $(top_srcdir)/t1reencode/t1reencode.1 $(top_srcdir)/t1testpage/t1testpage.1 $(top_srcdir)/ttftotype42/ttftotype42.1
	perl -pi -e 's/^(\U$(PACKAGE)\E) [\d.ab]+$$/$$1 $(VERSION)/;' $(top_srcdir)/README
	perl -pi -e 's/^Version: [\d.ab]+$$/Version: $(VERSION)/;' $(top_srcdir)/lcdf-typetools.spec

dist-hook:
	if test -f $(top_srcdir)/make-glyphtounicode.pl; then (cd $(top_srcdir); perl make-glyphtounicode.pl) > $(distdir)/glyphtounicode.tex; elif test -f $(top_srcdir)/glyphtounicode.tex; then cp -p $(top_srcdir)/glyphtounicode.tex $(distdir); fi

$(top_srcdir)/glyphtounicode.tex: $(top_srcdir)/glyphlist.txt $(top_srcdir)/texglyphlist.txt $(top_srcdir)/texglyphlist-g2u.txt $(top_srcdir)/make-glyphtounicode.pl
	cd $(top_srcdir); perl make-glyphtounicode.pl > glyphtounicode.tex

.PHONY: rpm liblcdf libefont cfftot1 mmafm mmpfb otfinfo otftotfm t1dotlessj t1lint t1rawafm t1reencode t1testpage ttftotype42

## Changes for TeX Live
##
EXTRA_DIST += lcdf-typetools-$(LCDF_TYPETOOLS_VERSION)-PATCHES

## Original files from lcdf-typetools distribution
##
EXTRA_DIST += Makefile.am.orig configure.ac.orig

## Tests
##
TESTS = tests/t1.test tests/tfm.test tests/ttf.test
EXTRA_DIST += $(TESTS)

## tests/t1.test
EXTRA_DIST += tests/t2c.enc tests/uhvrrc8a.pfb \
	tests/uhv_dl.pfa tests/uhv_dl.pfb tests/uhv_dl.ps \
	tests/uhv_re.afm tests/uhv_re.pfa
DISTCLEANFILES += uhv*
## tests/tfm.test
EXTRA_DIST += tests/antpolt-regular.otf tests/texnansx.enc \
	tests/Ant.enc tests/Ant.map tests/Ant.pfb tests/Ant.pl
DISTCLEANFILES += Ant* a_enhg3c.enc
## tests/ttf.test
EXTRA_DIST += tests/FonetikaDaniaIwonaeBold.ttf tests/texmfhome.otf \
	tests/Fon.post tests/Fon.t42 \
	tests/tmf.info tests/tmf.pfa tests/tmf.pfb tests/tmf.tables
DISTCLEANFILES += Fon* tmf.*

