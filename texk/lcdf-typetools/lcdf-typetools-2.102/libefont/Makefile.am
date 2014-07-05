## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS = foreign

noinst_LIBRARIES = libefont.a

libefont_a_SOURCES = \
	afm.cc \
	afmparse.cc \
	afmw.cc \
	amfm.cc \
	cff.cc \
	encoding.cc \
	findmet.cc \
	metrics.cc \
	otf.cc \
	otfcmap.cc \
	otfdata.cc \
	otfdescrip.cc \
	otfgpos.cc \
	otfgsub.cc \
	otfname.cc \
	otfos2.cc \
	otfpost.cc \
	pairop.cc \
	psres.cc \
	t1bounds.cc \
	t1cs.cc \
	t1csgen.cc \
	t1interp.cc \
	t1item.cc \
	t1font.cc \
	t1fontskel.cc \
	t1mm.cc \
	t1rw.cc \
	t1unparser.cc \
	ttfcs.cc \
	ttfhead.cc \
	ttfkern.cc

libefont_a_LIBADD = @TEMPLATE_OBJS@
CLEANFILES = @TEMPLATE_OBJS@

AM_CPPFLAGS = -I$(srcdir)/../include
