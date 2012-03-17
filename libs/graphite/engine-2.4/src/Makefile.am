AUTOMAKE_OPTIONS = 1.6

AM_CPPFLAGS = -I$(top_srcdir)/include/graphite -I$(top_srcdir)/src/generic
AM_CPPFLAGS += -I$(top_srcdir)/src/font -I$(top_srcdir)/src/segment 
#AM_CPPFLAGS += $(FT_CFLAGS) $(XFT_CFLAGS)
#AM_CPPFLAGS += -D_UNICODE -DUNICODE -DNO_ASM

lib_LTLIBRARIES = libgraphite.la

libgraphite_la_LDFLAGS = -version-info 4:0:0

#EXTRA_DIST=textsource
EXTRA_DIST=
graphiteinclude_HEADERS = 
graphiteincludedir = $(includedir)/graphite
include ../include/Makefile.am

libgraphite_la_SOURCES = 
include generic/Makefile.am
include font/Makefile.am
include segment/Makefile.am
include painter/Makefile.am
