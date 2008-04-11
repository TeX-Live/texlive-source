## pdftexlib.mk - Makefile fragment for libraries used by pdf[ex]tex.
# Public domain.

# The pdf*tex programs depend on a number of libraries.
# Include dependencies to get the built if we don't do make
# from the top-level directory.

Makefile: pdftexdir/pdftexlib.mk

# libpdf itself
pdflib = pdftexdir/libpdf.a
pdflib_sources = $(srcdir)/pdftexdir/*.c $(srcdir)/pdftexdir/*.cc \
	$(srcdir)/pdftexdir/*.h

pdftexdir/libpdf.a: $(pdflib_sources) pdftexdir/pdftexextra.h
	cd pdftexdir && $(MAKE) $(common_makeargs) libpdf.a


# Convenience variables.

pdftexlibs = $(pdflib) $(LDLIBPNG) $(LDZLIB) $(LDLIBXPDF) $(LIBMD5DEP) $(LDLIBOBSD)
pdftexlibsdep = $(pdflib) $(LIBPNGDEP) $(ZLIBDEP) $(LIBXPDFDEP) $(LIBMD5DEP) $(LIBOBSDDEP)

## end of pdftexlib.mk - Makefile fragment for libraries used by pdf[ex]tex.
