## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS = foreign check-news

SUBDIRS = liblcdf libefont @SELECTED_SUBDIRS@
DIST_SUBDIRS = liblcdf libefont cfftot1 mmafm mmpfb otfinfo otftotfm \
	t1dotlessj t1lint t1rawafm t1reencode t1testpage ttftotype42

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

glyphlist_DATA = glyphlist.txt texglyphlist.txt
enc_DATA = 7t.enc

liblcdf libefont:
	cd $@ && $(MAKE)
libefont: liblcdf
cfftot1 mmafm mmpfb otfinfo otftotfm t1dotlessj t1lint t1rawafm \
t1reencode t1testpage ttftotype42: liblcdf libefont
	cd $@ && $(MAKE)

versionize:
	perl -pi -e 's/^\.ds V.*/.ds V $(VERSION)/;' $(srcdir)/cfftot1/cfftot1.1 $(srcdir)/mmafm/mmafm.1 $(srcdir)/mmpfb/mmpfb.1 $(srcdir)/otfinfo/otfinfo.1 $(srcdir)/otftotfm/otftotfm.1 $(srcdir)/t1dotlessj/t1dotlessj.1 $(srcdir)/t1lint/t1lint.1 $(srcdir)/t1rawafm/t1rawafm.1 $(srcdir)/t1reencode/t1reencode.1 $(srcdir)/t1testpage/t1testpage.1 $(srcdir)/ttftotype42/ttftotype42.1
	perl -pi -e 's/^(\U$(PACKAGE)\E) [\d.ab]+$$/$$1 $(VERSION)/;' $(srcdir)/README
	perl -pi -e 's/^Version: [\d.ab]+$$/Version: $(VERSION)/;' $(srcdir)/lcdf-typetools.spec

dist-hook:
	if test -f $(srcdir)/make-glyphtounicode.pl; then (cd $(srcdir); perl make-glyphtounicode.pl) > $(distdir)/glyphtounicode.tex; elif test -f $(srcdir)/glyphtounicode.tex; then cp $(srcdir)/glyphtounicode.tex $(distdir); fi

$(srcdir)/glyphtounicode.tex: $(srcdir)/glyphlist.txt $(srcdir)/texglyphlist.txt $(srcdir)/texglyphlist-g2u.txt $(srcdir)/make-glyphtounicode.pl
	cd $(srcdir); perl make-glyphtounicode.pl > glyphtounicode.tex

.PHONY: rpm liblcdf libefont cfftot1 mmafm mmpfb otfinfo otftotfm t1dotlessj t1lint t1rawafm t1reencode t1testpage ttftotype42
