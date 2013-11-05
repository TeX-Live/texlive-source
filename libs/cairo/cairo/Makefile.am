## Proxy Makefile.am to install cairo headers for TeX Live.
##
##   Copyright (C) 2013 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
CAIRO_SRC = $(top_srcdir)/$(CAIRO_TREE)
CAIRO_BLD = $(top_builddir)

hdr_links = \
	$(CAIRO_SRC)/src/cairo.h \
	$(CAIRO_SRC)/src/cairo-deprecated.h \
	$(CAIRO_BLD)/cairo-features.h \
	$(CAIRO_SRC)/cairo-version.h

if CAIRO_HAS_XLIB_SURFACE
hdr_links += $(CAIRO_SRC)/src/cairo-xlib.h
endif CAIRO_HAS_XLIB_SURFACE

if CAIRO_HAS_XLIB_XRENDER_SURFACE
hdr_links += $(CAIRO_SRC)/src/cairo-xlib-xrender.h
endif CAIRO_HAS_XLIB_XRENDER_SURFACE

if CAIRO_HAS_XCB_SURFACE
hdr_links += $(CAIRO_SRC)/src/cairo-xcb.h
endif CAIRO_HAS_XCB_SURFACE

if CAIRO_HAS_QUARTZ_SURFACE
hdr_links += $(CAIRO_SRC)/src/cairo-quartz.h
endif CAIRO_HAS_QUARTZ_SURFACE

if CAIRO_HAS_QUARTZ_IMAGE_SURFACE
hdr_links += $(CAIRO_SRC)/src/cairo-quartz-image.h
endif CAIRO_HAS_QUARTZ_IMAGE_SURFACE

if CAIRO_HAS_OS2_SURFACE
hdr_links += $(CAIRO_SRC)/src/cairo-os2.h
endif CAIRO_HAS_OS2_SURFACE

if CAIRO_HAS_GL_SURFACE
hdr_links += $(CAIRO_SRC)/src/cairo-gl.h
endif CAIRO_HAS_GL_SURFACE

if CAIRO_HAS_SVG_SURFACE
hdr_links += $(CAIRO_SRC)/src/cairo-svg.h
endif CAIRO_HAS_SVG_SURFACE

include $(top_srcdir)/../../am/hdr_links.am

