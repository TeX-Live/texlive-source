## Proxy Makefile.am to build cairo for TeX Live.
##
##   Copyright (C) 2016 Karl Berry <tex-live@tug.org>
##   Copyright (C) 2013-2015 Peter Breitenlohner <tex-live@tug.org>
##   Copyright (C) 2012 Taco Hoekwater <taco@metatex.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
## We want to re-distribute the whole cairo source tree.
##
EXTRA_DIST = $(CAIRO_TREE)

## Changes applied to the original source tree
##
EXTRA_DIST += cairo-PATCHES

CAIRO_SRC = $(CAIRO_TREE)/src

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

SUBDIRS = . cairo

AM_CPPFLAGS = -I$(top_srcdir)/$(CAIRO_SRC) $(PIXMAN_INCLUDES) -DCAIRO_NO_MUTEX
AM_CFLAGS = $(CAIRO_ATTRIBUTE_FLAG) $(VISIBILITY_CFLAGS) # $(WARNING_CFLAGS)

noinst_LIBRARIES = libcairo.a

libcairo_a_SOURCES = \
	@CAIRO_TREE@/src/cairo-analysis-surface.c \
	@CAIRO_TREE@/src/cairo-arc.c \
	@CAIRO_TREE@/src/cairo-array.c \
	@CAIRO_TREE@/src/cairo-atomic.c \
	@CAIRO_TREE@/src/cairo-base64-stream.c \
	@CAIRO_TREE@/src/cairo-base85-stream.c \
	@CAIRO_TREE@/src/cairo-bentley-ottmann.c \
	@CAIRO_TREE@/src/cairo-bentley-ottmann-rectangular.c \
	@CAIRO_TREE@/src/cairo-bentley-ottmann-rectilinear.c \
	@CAIRO_TREE@/src/cairo-botor-scan-converter.c \
	@CAIRO_TREE@/src/cairo-boxes.c \
	@CAIRO_TREE@/src/cairo-boxes-intersect.c \
	@CAIRO_TREE@/src/cairo.c \
	@CAIRO_TREE@/src/cairo-cache.c \
	@CAIRO_TREE@/src/cairo-clip.c \
	@CAIRO_TREE@/src/cairo-clip-boxes.c \
	@CAIRO_TREE@/src/cairo-clip-polygon.c \
	@CAIRO_TREE@/src/cairo-clip-region.c \
	@CAIRO_TREE@/src/cairo-clip-surface.c \
	@CAIRO_TREE@/src/cairo-color.c \
	@CAIRO_TREE@/src/cairo-composite-rectangles.c \
	@CAIRO_TREE@/src/cairo-compositor.c \
	@CAIRO_TREE@/src/cairo-contour.c \
	@CAIRO_TREE@/src/cairo-damage.c \
	@CAIRO_TREE@/src/cairo-debug.c \
	@CAIRO_TREE@/src/cairo-default-context.c \
	@CAIRO_TREE@/src/cairo-device.c \
	@CAIRO_TREE@/src/cairo-error.c \
	@CAIRO_TREE@/src/cairo-fallback-compositor.c \
	@CAIRO_TREE@/src/cairo-fixed.c \
	@CAIRO_TREE@/src/cairo-font-face.c \
	@CAIRO_TREE@/src/cairo-font-face-twin.c \
	@CAIRO_TREE@/src/cairo-font-face-twin-data.c \
	@CAIRO_TREE@/src/cairo-font-options.c \
	@CAIRO_TREE@/src/cairo-freelist.c \
	@CAIRO_TREE@/src/cairo-freed-pool.c \
	@CAIRO_TREE@/src/cairo-gstate.c \
	@CAIRO_TREE@/src/cairo-hash.c \
	@CAIRO_TREE@/src/cairo-hull.c \
	@CAIRO_TREE@/src/cairo-image-compositor.c \
	@CAIRO_TREE@/src/cairo-image-info.c \
	@CAIRO_TREE@/src/cairo-image-source.c \
	@CAIRO_TREE@/src/cairo-image-surface.c \
	@CAIRO_TREE@/src/cairo-line.c \
	@CAIRO_TREE@/src/cairo-lzw.c \
	@CAIRO_TREE@/src/cairo-matrix.c \
	@CAIRO_TREE@/src/cairo-mask-compositor.c \
	@CAIRO_TREE@/src/cairo-mesh-pattern-rasterizer.c \
	@CAIRO_TREE@/src/cairo-mempool.c \
	@CAIRO_TREE@/src/cairo-misc.c \
	@CAIRO_TREE@/src/cairo-mono-scan-converter.c \
	@CAIRO_TREE@/src/cairo-mutex.c \
	@CAIRO_TREE@/src/cairo-no-compositor.c \
	@CAIRO_TREE@/src/cairo-observer.c \
	@CAIRO_TREE@/src/cairo-output-stream.c \
	@CAIRO_TREE@/src/cairo-paginated-surface.c \
	@CAIRO_TREE@/src/cairo-path-bounds.c \
	@CAIRO_TREE@/src/cairo-path.c \
	@CAIRO_TREE@/src/cairo-path-fill.c \
	@CAIRO_TREE@/src/cairo-path-fixed.c \
	@CAIRO_TREE@/src/cairo-path-in-fill.c \
	@CAIRO_TREE@/src/cairo-path-stroke.c \
	@CAIRO_TREE@/src/cairo-path-stroke-boxes.c \
	@CAIRO_TREE@/src/cairo-path-stroke-polygon.c \
	@CAIRO_TREE@/src/cairo-path-stroke-traps.c \
	@CAIRO_TREE@/src/cairo-path-stroke-tristrip.c \
	@CAIRO_TREE@/src/cairo-pattern.c \
	@CAIRO_TREE@/src/cairo-pen.c \
	@CAIRO_TREE@/src/cairo-polygon.c \
	@CAIRO_TREE@/src/cairo-polygon-intersect.c \
	@CAIRO_TREE@/src/cairo-polygon-reduce.c \
	@CAIRO_TREE@/src/cairo-raster-source-pattern.c \
	@CAIRO_TREE@/src/cairo-recording-surface.c \
	@CAIRO_TREE@/src/cairo-rectangle.c \
	@CAIRO_TREE@/src/cairo-rectangular-scan-converter.c \
	@CAIRO_TREE@/src/cairo-region.c \
	@CAIRO_TREE@/src/cairo-rtree.c \
	@CAIRO_TREE@/src/cairo-scaled-font.c \
	@CAIRO_TREE@/src/cairo-shape-mask-compositor.c \
	@CAIRO_TREE@/src/cairo-slope.c \
	@CAIRO_TREE@/src/cairo-spans.c \
	@CAIRO_TREE@/src/cairo-spans-compositor.c \
	@CAIRO_TREE@/src/cairo-spline.c \
	@CAIRO_TREE@/src/cairo-stroke-dash.c \
	@CAIRO_TREE@/src/cairo-stroke-style.c \
	@CAIRO_TREE@/src/cairo-surface.c \
	@CAIRO_TREE@/src/cairo-surface-clipper.c \
	@CAIRO_TREE@/src/cairo-surface-fallback.c \
	@CAIRO_TREE@/src/cairo-surface-observer.c \
	@CAIRO_TREE@/src/cairo-surface-offset.c \
	@CAIRO_TREE@/src/cairo-surface-snapshot.c \
	@CAIRO_TREE@/src/cairo-surface-subsurface.c \
	@CAIRO_TREE@/src/cairo-surface-wrapper.c \
	@CAIRO_TREE@/src/cairo-time.c \
	@CAIRO_TREE@/src/cairo-tor-scan-converter.c \
	@CAIRO_TREE@/src/cairo-tor22-scan-converter.c \
	@CAIRO_TREE@/src/cairo-clip-tor-scan-converter.c \
	@CAIRO_TREE@/src/cairo-toy-font-face.c \
	@CAIRO_TREE@/src/cairo-traps.c \
	@CAIRO_TREE@/src/cairo-tristrip.c \
	@CAIRO_TREE@/src/cairo-traps-compositor.c \
	@CAIRO_TREE@/src/cairo-unicode.c \
	@CAIRO_TREE@/src/cairo-user-font.c \
	@CAIRO_TREE@/src/cairo-version.c \
	@CAIRO_TREE@/src/cairo-wideint.c

if CAIRO_HAS_XLIB_SURFACE
libcairo_a_SOURCES += \
	@CAIRO_TREE@/src/cairo-xlib-display.c \
	@CAIRO_TREE@/src/cairo-xlib-core-compositor.c \
	@CAIRO_TREE@/src/cairo-xlib-fallback-compositor.c \
	@CAIRO_TREE@/src/cairo-xlib-render-compositor.c \
	@CAIRO_TREE@/src/cairo-xlib-screen.c \
	@CAIRO_TREE@/src/cairo-xlib-source.c \
	@CAIRO_TREE@/src/cairo-xlib-surface.c \
	@CAIRO_TREE@/src/cairo-xlib-surface-shm.c \
	@CAIRO_TREE@/src/cairo-xlib-visual.c \
	@CAIRO_TREE@/src/cairo-xlib-xcb-surface.c
endif CAIRO_HAS_XLIB_SURFACE

if CAIRO_HAS_XLIB_XRENDER_SURFACE
libcairo_a_SOURCES += \
	@CAIRO_TREE@/src/cairo-xcb-connection.c \
	@CAIRO_TREE@/src/cairo-xcb-connection-core.c \
	@CAIRO_TREE@/src/cairo-xcb-connection-render.c \
	@CAIRO_TREE@/src/cairo-xcb-connection-shm.c \
	@CAIRO_TREE@/src/cairo-xcb-resources.c \
	@CAIRO_TREE@/src/cairo-xcb-screen.c \
	@CAIRO_TREE@/src/cairo-xcb-shm.c \
	@CAIRO_TREE@/src/cairo-xcb-surface.c \
	@CAIRO_TREE@/src/cairo-xcb-surface-core.c \
	@CAIRO_TREE@/src/cairo-xcb-surface-render.c
endif CAIRO_HAS_XLIB_XRENDER_SURFACE

if CAIRO_HAS_XCB_SURFACE
libcairo_a_SOURCES += \
	@CAIRO_TREE@/src/cairo-xcb-connection.c \
	@CAIRO_TREE@/src/cairo-xcb-connection-core.c \
	@CAIRO_TREE@/src/cairo-xcb-connection-render.c \
	@CAIRO_TREE@/src/cairo-xcb-connection-shm.c \
	@CAIRO_TREE@/src/cairo-xcb-screen.c \
	@CAIRO_TREE@/src/cairo-xcb-shm.c \
	@CAIRO_TREE@/src/cairo-xcb-surface.c \
	@CAIRO_TREE@/src/cairo-xcb-surface-core.c \
	@CAIRO_TREE@/src/cairo-xcb-surface-render.c
endif CAIRO_HAS_XCB_SURFACE

if CAIRO_HAS_QUARTZ_SURFACE
libcairo_a_SOURCES += @CAIRO_TREE@/src/cairo-quartz-surface.c
endif CAIRO_HAS_QUARTZ_SURFACE

if CAIRO_HAS_QUARTZ_FONT
libcairo_a_SOURCES += @CAIRO_TREE@/src/cairo-quartz-font.c
endif CAIRO_HAS_QUARTZ_FONT

if CAIRO_HAS_QUARTZ_IMAGE_SURFACE
libcairo_a_SOURCES += @CAIRO_TREE@/src/cairo-quartz-image-surface.c
endif CAIRO_HAS_QUARTZ_IMAGE_SURFACE

if CAIRO_HAS_OS2_SURFACE
libcairo_a_SOURCES += @CAIRO_TREE@/src/cairo-os2-surface.c
endif CAIRO_HAS_OS2_SURFACE

if CAIRO_HAS_PNG_FUNCTIONS
libcairo_a_SOURCES += @CAIRO_TREE@/src/cairo-png.c
endif CAIRO_HAS_PNG_FUNCTIONS

if CAIRO_HAS_GL_SURFACE
libcairo_a_SOURCES += \
	@CAIRO_TREE@/src/cairo-gl-composite.c \
	@CAIRO_TREE@/src/cairo-gl-device.c \
	@CAIRO_TREE@/src/cairo-gl-dispatch.c \
	@CAIRO_TREE@/src/cairo-gl-glyphs.c \
	@CAIRO_TREE@/src/cairo-gl-gradient.c \
	@CAIRO_TREE@/src/cairo-gl-info.c \
	@CAIRO_TREE@/src/cairo-gl-operand.c \
	@CAIRO_TREE@/src/cairo-gl-shaders.c \
	@CAIRO_TREE@/src/cairo-gl-msaa-compositor.c \
	@CAIRO_TREE@/src/cairo-gl-spans-compositor.c \
	@CAIRO_TREE@/src/cairo-gl-traps-compositor.c \
	@CAIRO_TREE@/src/cairo-gl-source.c \
	@CAIRO_TREE@/src/cairo-gl-surface.c
endif CAIRO_HAS_GL_SURFACE

if CAIRO_HAS_SVG_SURFACE
libcairo_a_SOURCES += @CAIRO_TREE@/src/cairo-svg-surface.c
endif CAIRO_HAS_SVG_SURFACE

$(libcairo_a_OBJECTS): config.force

if build
check_PROGRAMS = cairotst
dist_check_SCRIPTS = cairo.test
TESTS = cairo.test
endif build
cairo.log: cairotst$(EXEEXT)

cairotst_SOURCES = cairotst.c

cairotst_CPPFLAGS = $(PIXMAN_INCLUDES) -Icairo

cairotst_DEPENDENCIES = $(PIXMAN_DEPEND)

LDADD = libcairo.a $(PIXMAN_LIBS)

## Rebuild pixman
@PIXMAN_RULE@

# Reconfig
reconfig_prereq = $(PIXMAN_DEPEND)
DISTCLEANFILES =

include $(srcdir)/../../am/reconfig.am

# Rebuild
rebuild_prereq =
rebuild_target = all
CLEANFILES =

include $(srcdir)/../../am/rebuild.am

