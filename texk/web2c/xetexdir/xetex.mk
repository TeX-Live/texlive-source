# Makefile fragment for XeTeX and web2c. -- Jonathan Kew -- Public domain.
# This fragment contains the parts of the makefile that are most likely to
# differ between releases of XeTeX.

Makefile: $(srcdir)/xetexdir/xetex.mk

# this should probably move to common.mk some day,
# but need to check possible effect on other programs
ALL_CXXFLAGS = @CXXFLAGS@

# We build xetex unless configure decides to skip it
xetex = @XETEX@ xetex

### Platform-specific defines and files to be built

# On Mac OS X:
@XETEX_MACOSX@ xetex_platform_o = XeTeX_mac.o XeTeXFontMgr_Mac.o
@XETEX_MACOSX@ xetex_platform_layout_o = XeTeXFontInst_Mac.o XeTeXFontInst_FT2.o
@XETEX_MACOSX@ xetex_platform_layout_cxx = XeTeXFontInst_Mac.cpp XeTeXFontInst_FT2.cpp
@XETEX_MACOSX@ xetex_images_o =

@XETEX_MACOSX@ XETEX_DEFINES = -DXETEX_MAC

@XETEX_MACOSX@ EXTRALIBS = -framework Carbon -framework Cocoa -framework QuickTime

@XETEX_MACOSX@ EXTRADEPS = 

# On non-Mac platforms:
@XETEX_GENERIC@ xetex_platform_o = XeTeXFontMgr_FC.o
@XETEX_GENERIC@ xetex_platform_layout_o = XeTeXFontInst_FT2.o
@XETEX_GENERIC@ xetex_platform_layout_cxx = XeTeXFontInst_FT2.cpp
@XETEX_GENERIC@ xetex_images_o = mfileio.o numbers.o pdfimage.o bmpimage.o jpegimage.o pngimage.o XeTeX_pic.o

@XETEX_GENERIC@ XETEX_DEFINES = -DXETEX_OTHER

@XETEX_GENERIC@ EXTRALIBS = @LDLIBXPDF@ @LDLIBPNG@ @LDFONTCONFIG@

@XETEX_GENERIC@ EXTRADEPS = @LIBXPDFDEP@ @LIBPNGDEP@

### end of platform-specific setup

LDLIBXPDF=@LDLIBXPDF@
LIBXPDFCPPFLAGS=@LIBXPDFCPPFLAGS@
LIBXPDFDEP=@LIBXPDFDEP@

LIBXPDFDIR=../../libs/xpdf
LIBXPDFSRCDIR=$(srcdir)/$(LIBXPDFDIR)

LDLIBPNG=@LDLIBPNG@
LIBPNGCPPFLAGS=@LIBPNGCPPFLAGS@
LIBPNGDEP=@LIBPNGDEP@

LIBPNGDIR=../../libs/libpng
LIBPNGSRCDIR=$(srcdir)/$(LIBPNGDIR)

LDFREETYPE2 = @LDFREETYPE2@
FTFLAGS =  @FREETYPE2CPPFLAGS@
FREETYPE2DEP = @FREETYPE2DEP@

FREETYPE2DIR = ../../libs/freetype2
FREETYPE2SRCDIR = $(srcdir)/$(FREETYPE2DIR)

TECKITFLAGS = @TECKITCPPFLAGS@
LDTECKIT = @LDTECKIT@
TECKITDEP = @TECKITDEP@

TECKITDIR=../../libs/teckit
TECKITSRCDIR=$(srcdir)/$(TECKITDIR)

ICUFLAGS = @ICUCPPFLAGS@
LDICU = @LDICU@
ICUDEP = @ICUDEP@

ICUDIR=../../libs/icu-xetex
ICUSRCDIR=$(srcdir)/$(ICUDIR)

ICUCFLAGS = @ICUCPPFLAGS@ -DLE_USE_CMEMORY

ZLIBCPPFLAGS = @ZLIBCPPFLAGS@
LDZLIB = @LDZLIB@

ZLIBDIR = ../../libs/zlib
ZLIBSRCDIR = $(srcdir)/$(ZLIBDIR)

FONTCONFIGCPPFLAGS = @FONTCONFIGCPPFLAGS@
FONTCONFIGLDFLAGS  = @FONTCONFIGLDFLAGS@

GRAPHITEDIR = ../../libs/graphite-engine
GRAPHITESRCDIR = $(srcdir)/$(GRAPHITEDIR)

GRAPHITEFLAGS = @GRAPHITECPPFLAGS@
LDGRAPHITE = @LDGRAPHITE@
GRAPHITEDEP = @GRAPHITEDEP@

xetexlibs = $(LDICU) $(LDTECKIT) $(LDFREETYPE2) $(LDGRAPHITE) $(LDZLIB)

# Font-related headers
XeTeXFontHdrs = \
	$(srcdir)/xetexdir/FontTableCache.h \
	$(srcdir)/xetexdir/XeTeXFontInst.h \
	$(srcdir)/xetexdir/XeTeXFontInst_FT2.h \
	$(srcdir)/xetexdir/XeTeXFontInst_Mac.h \
	$(srcdir)/xetexdir/XeTeXFontMgr.h \
	$(srcdir)/xetexdir/XeTeXFontMgr_FC.h \
	$(srcdir)/xetexdir/XeTeXFontMgr_Mac.h \
	$(srcdir)/xetexdir/XeTeXLayoutInterface.h \
	$(srcdir)/xetexdir/XeTeXOTLayoutEngine.h
	
# Image-related headers
XeTeXImageHdrs = \
	$(srcdir)/xetexdir/bmpimage.h \
	$(srcdir)/xetexdir/jpegimage.h \
	$(srcdir)/xetexdir/pdfimage.h \
	$(srcdir)/xetexdir/pngimage.h

# Extract xetex version
xetexdir/xetex.version: $(srcdir)/xetexdir/xetex.ch
	test -d xetexdir || mkdir xetexdir
	grep '^@d XeTeX_version_string==' $(srcdir)/xetexdir/xetex.ch \
	  | sed "s/^.*'-//;s/'.*$$//" >xetexdir/xetex.version

# Extract etex version
xetexdir/etex.version: $(srcdir)/etexdir/etex.ch
	test -d xetexdir || mkdir xetexdir
	grep '^@d eTeX_version_string==' $(srcdir)/etexdir/etex.ch \
	  | sed "s/^.*'-//;s/'.*$$//" >xetexdir/etex.version

# The C sources.
xetex_c = xetexini.c xetex0.c xetex1.c xetex2.c
xetex_o = xetexini.o xetex0.o xetex1.o xetex2.o xetexextra.o
xetex_add_o = trans.o XeTeX_ext.o xetex_pool.o $(xetex_platform_o) $(xetex_o-with_synctex)

# these compilations require the path to TECkit headers;
# just setting it in XCFLAGS doesn't seem to work when we're called
# recursively from "make world" etc
xetexini.o: xetexini.c $(srcdir)/xetexdir/XeTeX_ext.h
	$(compile) $(TECKITFLAGS) $(FTFLAGS) $(ALL_CFLAGS) $(XETEX_DEFINES) -c $< -o $@
xetex0.o: xetex0.c $(srcdir)/xetexdir/XeTeX_ext.h
	$(compile) $(TECKITFLAGS) $(FTFLAGS) $(ALL_CFLAGS) $(XETEX_DEFINES) -c $< -o $@
xetex1.o: xetex1.c $(srcdir)/xetexdir/XeTeX_ext.h
	$(compile) $(TECKITFLAGS) $(FTFLAGS) $(ALL_CFLAGS) $(XETEX_DEFINES) -c $< -o $@
xetex2.o: xetex2.c $(srcdir)/xetexdir/XeTeX_ext.h
	$(compile) $(TECKITFLAGS) $(FTFLAGS) $(ALL_CFLAGS) $(XETEX_DEFINES) -c $< -o $@
xetexextra.o: xetexextra.c $(srcdir)/xetexdir/XeTeX_ext.h
	$(compile) $(TECKITFLAGS) $(FTFLAGS) $(ALL_CFLAGS) $(XETEX_DEFINES) -c $< -o $@
xetex_pool.o: xetex_pool.c $(srcdir)/xetexdir/XeTeX_ext.h
	$(compile) $(TECKITFLAGS) $(FTFLAGS) $(ALL_CFLAGS) $(XETEX_DEFINES) -c $< -o $@

# image support
mfileio.o: $(srcdir)/xetexdir/mfileio.c $(srcdir)/xetexdir/mfileio.h
	$(compile) $(ALL_CFLAGS) $(FTFLAGS) -c $< -o $@

numbers.o: $(srcdir)/xetexdir/numbers.c $(srcdir)/xetexdir/numbers.h
	$(compile) $(ALL_CFLAGS) $(FTFLAGS) -c $< -o $@

bmpimage.o: $(srcdir)/xetexdir/bmpimage.c $(srcdir)/xetexdir/bmpimage.h
	$(compile) $(ALL_CFLAGS) $(FTFLAGS) -c $< -o $@

jpegimage.o: $(srcdir)/xetexdir/jpegimage.c $(srcdir)/xetexdir/jpegimage.h
	$(compile) $(ALL_CFLAGS) $(FTFLAGS) -c $< -o $@

pngimage.o: $(srcdir)/xetexdir/pngimage.c $(srcdir)/xetexdir/pngimage.h
	$(compile) $(ALL_CFLAGS) $(FTFLAGS) $(LIBPNGCPPFLAGS) $(ZLIBCPPFLAGS) -c $< -o $@

pdfimage.o: $(srcdir)/xetexdir/pdfimage.cpp $(srcdir)/xetexdir/pdfimage.h
	$(CXX) $(ALL_CFLAGS) $(FTFLAGS) $(LIBXPDFCPPFLAGS) -c $< -o $@

XeTeX_pic.o: $(srcdir)/xetexdir/XeTeX_pic.c $(srcdir)/xetexdir/XeTeX_ext.h $(XeTeXImageHdrs)
	$(compile) $(TECKITFLAGS) $(FTFLAGS) $(ALL_CFLAGS) $(XETEX_DEFINES) -c $< -o $@

# Layout library
xetex_ot_layout_o = \
		XeTeXFontMgr.o \
		XeTeXLayoutInterface.o XeTeXOTLayoutEngine.o \
		XeTeXFontInst.o cmaps.o FontTableCache.o \
		XeTeXOTMath.o \
		XeTeXGrLayout.o \
		$(xetex_platform_layout_o) 

XeTeXLayoutInterface.o: $(srcdir)/xetexdir/XeTeXLayoutInterface.cpp $(XeTeXFontHdrs)
	$(CXX) $(ICUCFLAGS) $(FTFLAGS) $(GRAPHITEFLAGS) $(FONTCONFIGCPPFLAGS) $(ALL_CXXFLAGS) $(XETEX_DEFINES) -c $< -o $@
XeTeXOTLayoutEngine.o: $(srcdir)/xetexdir/XeTeXOTLayoutEngine.cpp $(XeTeXFontHdrs)
	$(CXX) $(ICUCFLAGS) $(FTFLAGS) $(FONTCONFIGCPPFLAGS) $(ALL_CXXFLAGS) $(XETEX_DEFINES) -c $< -o $@

XeTeXFontMgr.o: $(srcdir)/xetexdir/XeTeXFontMgr.cpp  $(XeTeXFontHdrs)
	$(CXX) $(ICUCFLAGS) $(FTFLAGS) $(FONTCONFIGCPPFLAGS) $(ALL_CXXFLAGS) $(XETEX_DEFINES) -c $< -o $@
XeTeXFontMgr_FC.o: $(srcdir)/xetexdir/XeTeXFontMgr_FC.cpp  $(XeTeXFontHdrs)
	$(CXX) $(ICUCFLAGS) $(FTFLAGS) $(FONTCONFIGCPPFLAGS) $(ALL_CXXFLAGS) $(XETEX_DEFINES) -c $< -o $@

XeTeXFontMgr_Mac.o: $(srcdir)/xetexdir/XeTeXFontMgr_Mac.mm  $(XeTeXFontHdrs)
	$(CXX) -ObjC++ $(ICUCFLAGS) $(FTFLAGS) $(ALL_CXXFLAGS) $(XETEX_DEFINES) -c $< -o $@

cmaps.o: $(srcdir)/xetexdir/cmaps.cpp
	$(CXX) $(ICUCFLAGS) $(ALL_CXXFLAGS) $(XETEX_DEFINES) -c $< -o $@
FontTableCache.o: $(srcdir)/xetexdir/FontTableCache.cpp
	$(CXX) $(ICUCFLAGS) $(ALL_CXXFLAGS) $(XETEX_DEFINES) -c $< -o $@

XeTeXFontInst.o: $(srcdir)/xetexdir/XeTeXFontInst.cpp $(XeTeXFontHdrs)
	$(CXX) $(ICUCFLAGS) $(FTFLAGS) $(FONTCONFIGCPPFLAGS) $(ALL_CXXFLAGS) $(XETEX_DEFINES) -c $< -o $@
XeTeXFontInst_Mac.o: $(srcdir)/xetexdir/XeTeXFontInst_Mac.cpp $(XeTeXFontHdrs)
	$(CXX) $(ICUCFLAGS) $(ALL_CXXFLAGS) $(XETEX_DEFINES) -c $< -o $@
XeTeXFontInst_FT2.o: $(srcdir)/xetexdir/XeTeXFontInst_FT2.cpp $(XeTeXFontHdrs)
	$(CXX) $(ICUCFLAGS) $(FTFLAGS) $(FONTCONFIGCPPFLAGS) $(ALL_CXXFLAGS) $(XETEX_DEFINES) -c $< -o $@

XeTeXOTMath.o: $(srcdir)/xetexdir/XeTeXOTMath.cpp $(XeTeXFontHdrs)
	$(CXX) $(ICUCFLAGS) $(FTFLAGS) $(FONTCONFIGCPPFLAGS) $(ALL_CXXFLAGS) $(XETEX_DEFINES) -c $< -o $@

XeTeXGrLayout.o: $(srcdir)/xetexdir/XeTeXGrLayout.cpp $(srcdir)/xetexdir/XeTeXGrLayout.h \
		$(XeTeXFontHdrs) $(GRAPHITESRCDIR)/include/graphite/Font.h
	$(CXX) $(ICUCFLAGS) $(FTFLAGS) $(FONTCONFIGCPPFLAGS) $(GRAPHITEFLAGS) $(ALL_CXXFLAGS) $(XETEX_DEFINES) -c $< -o $@

# special rules for files that need the TECkit headers as well
XeTeX_ext.o: $(srcdir)/xetexdir/XeTeX_ext.c xetexd.h
	$(compile) $(ICUCFLAGS) $(FTFLAGS) $(TECKITFLAGS) $(LIBPNGCPPFLAGS) $(LIBXPDFCPPFLAGS) $(ZLIBCPPFLAGS) $(ALL_CFLAGS) $(XETEX_DEFINES) -c $< -o $@
XeTeX_mac.o: $(srcdir)/xetexdir/XeTeX_mac.c xetexd.h
	$(compile) $(ICUCFLAGS) $(TECKITFLAGS) $(ALL_CFLAGS) $(XETEX_DEFINES) -c $< -o $@

trans.o: $(srcdir)/xetexdir/trans.c
	$(compile) $(ALL_CFLAGS) $(XETEX_DEFINES) -c $< -o $@

# sync
synctex-xe.o: synctex-xe.c
	$(compile) $(ALL_CFLAGS) $(TECKITFLAGS) $(FTFLAGS) $(XETEX_DEFINES) -c $< -o $@

# Making xetex.
xetex: $(xetex_o) $(xetex_add_o) $(xetex_images_o) $(xetex_ot_layout_o) \
		$(GRAPHITEDEP) $(TECKITDEP) $(FREETYPE2DEP) $(ICUDEP) $(EXTRADEPS)
	@CXXHACKLINK@ $(xetex_o) $(xetex_add_o) $(xetex_images_o) $(xetex_ot_layout_o) \
	$(FONTCONFIGLDFLAGS) $(socketlibs) $(xetexlibs) $(EXTRALIBS) \
	@CXXHACKLDLIBS@ @CXXLDEXTRA@ @PTHREAD_CFLAGS@ @PTHREAD_LIBS@

# C file dependencies
$(xetex_c) xetexcoerce.h xetexd.h: xetex.p $(web2c_texmf)
	$(web2c) xetex
	$(xetexd.h-with_synctex)

xetexextra.c: xetexd.h lib/texmfmp.c xetexdir/xetexextra.h
	sed s/TEX-OR-MF-OR-MP/xetex/ $(srcdir)/lib/texmfmp.c >$@
xetexdir/xetexextra.h: xetexdir/xetexextra.in xetexdir/xetex.version xetexdir/etex.version
	test -d xetexdir || mkdir xetexdir
	sed -e s/XETEX-VERSION/`cat xetexdir/xetex.version`/ \
	    -e s/ETEX-VERSION/`cat xetexdir/etex.version`/ \
	  $(srcdir)/xetexdir/xetexextra.in >$@

xetex_pool.c: xetex.pool
	perl $(srcdir)/xetexdir/pool2c.pl $< $@

# Tangling
xetex.p xetex.pool: ./otangle xetex.web
	./otangle xetex.web

# Generation of the web file.
#   Sources for xetex.web:
xetex_web_srcs = $(srcdir)/tex.web \
  $(srcdir)/etexdir/etex.ch \
  $(srcdir)/etexdir/tex.ch0 \
  $(srcdir)/tex.ch \
  $(srcdir)/etexdir/tex.ch1 \
  $(srcdir)/etexdir/tex.ech \
  $(xetex_ch_srcs-with_synctex) \
  $(srcdir)/xetexdir/xetex.ch \
  $(xetex_post_ch_srcs-with_synctex)
xetex.web: tie xetexdir/xetex.mk $(xetex_web_srcs)
	$(TIE) -m xetex.web $(xetex_web_srcs)

################## FIXME: the rest of this isn't properly updated for xetex yet...
##################        e.g., we don't have real xetex tests to run!

# Tests...
#check: @XETEX@ xetex-check
#xetex-check: etrip xetex.fmt
# Test truncation (but don't bother showing the warning msg).
#	./xetex --progname=xetex --output-comment="`cat $(srcdir)/PROJECTS`" \
#	  $(srcdir)/tests/hello 2>/dev/null \
#	  && ./dvitype hello.dvi | grep olaf@infovore.xs4all.nl >/dev/null
# \openout should show up in \write's.
#	./xetex --progname=xetex $(srcdir)/tests/openout && grep xfoo openout.log
# one.two.tex -> one.two.log
#	./xetex --progname=xetex $(srcdir)/tests/one.two && ls -l one.two.log
# uno.dos -> uno.log
#	./xetex --progname=xetex $(srcdir)/tests/uno.dos && ls -l uno.log
#	./xetex --progname=xetex $(srcdir)/tests/just.texi && ls -l just.log
#	-./xetex --progname=xetex $(srcdir)/tests/batch.tex
#	./xetex --progname=xetex --shell $(srcdir)/tests/write18 | grep echo
#	./xetex --mltex --progname=xeinitex $(srcdir)/tests/mltextst
#	-./xetex --progname=xetex </dev/null
#	-PATH=`pwd`:$(kpathsea_dir):$(kpathsea_srcdir):$$PATH \
#	  WEB2C=$(kpathsea_srcdir) TMPDIR=.. \
#	  ./xetex --progname=xetex '\nonstopmode\font\foo=nonesuch\end'

# Cleaning up.
clean:: xetex-clean
xetex-clean: # etrip-clean
	$(LIBTOOL) --mode=clean $(RM) xetex
	rm -f $(xetex_o) $(xetex_c) xetexextra.c xetex_pool.c xetexcoerce.h xetexd.h
	rm -f xetexdir/xetexextra.h xetexdir/xetex.version
	rm -f xetex.p xetex.pool xetex.web
	rm -f xetex.fmt xetex.log
	rm -f hello.dvi hello.log xfoo.out openout.log one.two.log uno.log
	rm -f just.log batch.log write18.log mltextst.log texput.log
	rm -f missfont.log
	rm -rf tfm

# Dumps
all_xefmts = xetex.fmt $(xefmts)

dumps: @XETEX@ xefmts
xefmts: $(all_xefmts)

xefmtdir = $(web2cdir)/xetex
$(xefmtdir)::
	$(SHELL) $(top_srcdir)/../mkinstalldirs ${DESTDIR}$(xefmtdir)

xetex.fmt: xetex
	$(dumpenv) $(MAKE) progname=xetex files="xetex.ini unicode-letters.tex plain.tex cmr10.tfm" prereq-check
	$(dumpenv) ./xetex --progname=xetex --jobname=xetex --ini \*\\input xetex.ini \\dump </dev/null

xelatex.fmt: xetex
	$(dumpenv) $(MAKE) progname=xelatex files="xelatex.ini unicode-letters.tex latex.ltx" prereq-check
	$(dumpenv) ./xetex --progname=xelatex --jobname=xelatex --ini \*\\input xelatex.ini </dev/null


# Install
install-xetex: install-xetex-exec install-xetex-data
install-xetex-exec: install-xetex-programs install-xetex-links
install-xetex-data: @FMU@ install-xetex-dumps
install-xetex-dumps: install-xetex-fmts

install-programs: @XETEX@ install-xetex-programs
install-xetex-programs: xetex $(bindir)
	for p in xetex; do $(INSTALL_LIBTOOL_PROG) $$p ${DESTDIR}$(bindir); done

install-links: @XETEX@ install-xetex-links
install-xetex-links: install-xetex-programs
	#cd $(bindir) && (rm -f xeinitex xevirtex; \
	#  $(LN) xetex xeinitex; $(LN) xetex xevirtex)

install-fmts: @XETEX@ install-xetex-fmts
install-xetex-fmts: xefmts $(xefmtdir)
	xefmts="$(all_xefmts)"; \
	  for f in $$xefmts; do $(INSTALL_DATA) $$f ${DESTDIR}$(xefmtdir)/$$f; done
	xefmts="$(xefmts)"; \
	  for f in $$xefmts; do base=`basename $$f .fmt`; \
	    (cd ${DESTDIR}$(bindir) && (rm -f $$base; $(LN) xetex $$base)); done

install-data:: @XETEX@ install-xetex-data

# end of xetex.mk
