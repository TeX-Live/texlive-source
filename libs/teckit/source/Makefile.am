teckitdir = $(top_srcdir)/source

libTECkit_Compiler_la_SOURCES = $(teckitdir)/Compiler.cpp 
libTECkit_Compiler_la_SOURCES += $(teckitdir)/UnicodeNames.cpp

libTECkit_la_SOURCES = $(teckitdir)/Engine.cpp

noinst_HEADERS = $(teckitdir)/Compiler.h
noinst_HEADERS += $(teckitdir)/Engine.h
noinst_HEADERS += $(teckitdir)/ulong_chartraits.h
noinst_HEADERS += $(teckitdir)/TECkit_Format.h

EXTRA_DIST = $(teckitdir)/NormalizationData.c

include $(top_srcdir)/source/Public-headers/Makefile.am
