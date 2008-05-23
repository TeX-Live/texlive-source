# $Id$
# Libraries common to luatex and pdftex.
# Public domain.

# libz
ZLIBDIR=../../libs/zlib
ZLIBSRCDIR=$(srcdir)/$(ZLIBDIR)
ZLIBDEP = @ZLIBDEP@
LDZLIB = @LDZLIB@

$(ZLIBDIR)/libz.a: $(ZLIBSRCDIR)
	cd $(ZLIBDIR) && $(MAKE) $(common_makeargs) libz.a

# libpng
LIBPNGDIR=../../libs/libpng
LIBPNGSRCDIR=$(srcdir)/$(LIBPNGDIR)
LIBPNGDEP = @LIBPNGDEP@
LDLIBPNG = @LDLIBPNG@

$(LIBPNGDIR)/libpng.a: $(LIBPNGSRCDIR)/*.c
	cd $(LIBPNGDIR) && $(MAKE) $(common_makeargs) libpng.a

# libxpdf
LIBXPDFDIR=../../libs/xpdf
LIBXPDFSRCDIR=$(srcdir)/$(LIBXPDFDIR)
LIBXPDFDEP = @LIBXPDFDEP@
LDLIBXPDF = @LDLIBXPDF@

$(LIBXPDFDIR)/fofi/libfofi.a: $(LIBXPDFSRCDIR)/fofi/*.cc \
	$(LIBXPDFSRCDIR)/fofi/*.h
	cd $(LIBXPDFDIR)/fofi; $(MAKE) $(common_makeargs) libfofi.a
$(LIBXPDFDIR)/goo/libGoo.a: $(LIBXPDFSRCDIR)/goo/*.cc \
	$(LIBXPDFSRCDIR)/goo/*.c $(LIBXPDFSRCDIR)/goo/*.h
	cd $(LIBXPDFDIR)/goo; $(MAKE) $(common_makeargs) libGoo.a
$(LIBXPDFDIR)/xpdf/libxpdf.a: $(LIBXPDFSRCDIR)/xpdf/*.cc \
	$(LIBXPDFSRCDIR)/xpdf/*.h
	cd $(LIBXPDFDIR)/xpdf; $(MAKE) $(common_makeargs) libxpdf.a

# md5
LIBMD5DIR=../../libs/md5
LIBMD5SRCDIR=$(srcdir)/$(LIBMD5DIR)
LIBMD5DEP=$(LIBMD5DIR)/md5.o

$(LIBMD5DEP): $(LIBMD5SRCDIR)/md5.c $(LIBMD5SRCDIR)/md5.h
clean:: md5lib-clean
md5lib-clean:
	rm -f $(LIBMD5DEP)

# obsdcompat
LIBOBSDDIR=../../libs/obsdcompat
LIBOBSDSRCDIR=$(srcdir)/$(LIBOBSDDIR)
LIBOBSDDEP=@LIBOBSDDEP@
LDLIBOBSD=@LDLIBOBSD@

$(LIBOBSDDIR)/libopenbsd-compat.a: $(LIBOBSDSRCDIR)/*.c $(LIBOBSDSRCDIR)/*.h
# common_makeargs = $(MFLAGS) CC='$(CC)' CFLAGS='$(CFLAGS)' LDFLAGS='$(LDFLAGS)' $(XMAKEARGS)
# CFLAGS setzt libopenbsd-compat selbst, nicht durchreichen!
	cd $(LIBOBSDDIR); $(MAKE) $(MFLAGS) $(XMAKEARGS) libopenbsd-compat.a

