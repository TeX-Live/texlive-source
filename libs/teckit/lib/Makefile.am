AUTOMAKE_OPTIONS = 1.6

ZLIBDIR = $(top_builddir)/../zlib
ZLIBSRCDIR = $(top_srcdir)/../zlib

ZLIBCPPFLAGS = @ZLIBCPPFLAGS@

AM_CPPFLAGS  = -I$(top_srcdir)/source/Public-headers $(ZLIBCPPFLAGS)
AM_CFLAGS    = $(AM_CPPFLAGS)

lib_LTLIBRARIES = libTECkit_Compiler.la libTECkit.la

include ../source/Makefile.am

