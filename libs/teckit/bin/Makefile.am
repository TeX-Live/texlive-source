AUTOMAKE_OPTIONS = 1.6

bin_PROGRAMS = teckit_compile txtconv # sfconv

AM_CPPFLAGS  = -I$(top_srcdir)/source/Public-headers

expatdir = $(top_srcdir)/SFconv/expat

ZLIBSRCDIR = $(top_srcdir)/../zlib
ZLIBDIR = $(top_builddir)/../zlib

#if SYSTEM_EXPAT
#EXPAT_CFLAGS = -DXML_DTD
#EXPAT_SOURCES =
#else
#EXPAT_CFLAGS = -I$(expatdir)/xmlparse -I$(expatdir)/xmltok -DXML_DTD
#EXPAT_SOURCES  = $(expatdir)/xmlparse/xmlparse.c
#EXPAT_SOURCES += $(expatdir)/xmlparse/hashtable.c
#EXPAT_SOURCES += $(expatdir)/xmltok/xmlrole.c
#EXPAT_SOURCES += $(expatdir)/xmltok/xmltok.c
#endif

AM_CPPFLAGS  += $(EXPAT_CFLAGS)
AM_CFLAGS    = $(AM_CPPFLAGS)


teckit_compile_SOURCES = $(top_srcdir)/source/Sample-tools/TECkit_Compile.cpp

txtconv_SOURCES = $(top_srcdir)/source/Sample-tools/TxtConv.cpp

# sfconv_SOURCES  = $(top_srcdir)/SFconv/SFconv.cpp
# sfconv_SOURCES += $(top_srcdir)/SFconv/ConvertUTF.c
# sfconv_SOURCES += $(EXPAT_SOURCES)

teckit_compile_LDADD = $(top_builddir)/lib/libTECkit_Compiler.la @LDZLIB@

txtconv_LDADD = $(top_builddir)/lib/libTECkit.la @LDZLIB@

# sfconv_LDADD = $(top_builddir)/lib/libTECkit.la

