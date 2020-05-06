teckitdir  = $(top_srcdir)/source
ZLIBSRCDIR = $(top_srcdir)/@ZLIBDIR@

if SYSTEM_ZLIB
ZLIB_CFLAGS = @ZLIBCPPFLAGS@
ZLIB_SOURCES =
else
ZLIB_CFLAGS = -I$(ZLIBSRCDIR)
ZLIB_SOURCES  = @top_builddir@/@ZLIBDIR@/adler32.c
ZLIB_SOURCES += @top_builddir@/@ZLIBDIR@/compress.c
ZLIB_SOURCES += @top_builddir@/@ZLIBDIR@/crc32.c
ZLIB_SOURCES += @top_builddir@/@ZLIBDIR@/deflate.c
ZLIB_SOURCES += @top_builddir@/@ZLIBDIR@/gzclose.c
ZLIB_SOURCES += @top_builddir@/@ZLIBDIR@/gzlib.c
ZLIB_SOURCES += @top_builddir@/@ZLIBDIR@/gzread.c
ZLIB_SOURCES += @top_builddir@/@ZLIBDIR@/gzwrite.c
ZLIB_SOURCES += @top_builddir@/@ZLIBDIR@/infback.c
ZLIB_SOURCES += @top_builddir@/@ZLIBDIR@/inffast.c
ZLIB_SOURCES += @top_builddir@/@ZLIBDIR@/inflate.c
ZLIB_SOURCES += @top_builddir@/@ZLIBDIR@/inftrees.c
ZLIB_SOURCES += @top_builddir@/@ZLIBDIR@/trees.c
ZLIB_SOURCES += @top_builddir@/@ZLIBDIR@/uncompr.c
ZLIB_SOURCES += @top_builddir@/@ZLIBDIR@/zutil.c
ZLIB_SOURCES += @top_builddir@/@ZLIBDIR@/crc32.h
ZLIB_SOURCES += @top_builddir@/@ZLIBDIR@/deflate.h
ZLIB_SOURCES += @top_builddir@/@ZLIBDIR@/gzguts.h
ZLIB_SOURCES += @top_builddir@/@ZLIBDIR@/inffast.h
ZLIB_SOURCES += @top_builddir@/@ZLIBDIR@/inffixed.h
ZLIB_SOURCES += @top_builddir@/@ZLIBDIR@/inflate.h
ZLIB_SOURCES += @top_builddir@/@ZLIBDIR@/inftrees.h
ZLIB_SOURCES += @top_builddir@/@ZLIBDIR@/trees.h
ZLIB_SOURCES += @top_builddir@/@ZLIBDIR@/zconf.h
ZLIB_SOURCES += @top_builddir@/@ZLIBDIR@/zlib.h
ZLIB_SOURCES += @top_builddir@/@ZLIBDIR@/zutil.h
endif

AM_CPPFLAGS  += $(ZLIBCPPFLAGS)
AM_CFLAGS    = $(AM_CPPFLAGS)

if OLD_LIB_NAMES
OLD_LIB_LDFLAGS = -module -avoid-version -Wl,--add-stdcall-alias
TECkit_x86_la_CFLAGS = $(ZLIB_CFLAGS)
TECkit_x86_la_SOURCES = @top_builddir@/source/Engine.cpp @top_build_prefix@source/Engine_ver.rc $(ZLIB_SOURCES)
TECkit_x86_la_LDFLAGS = $(LDFLAGS) $(OLD_LIB_LDFLAGS) -no-undefined $(LDZLIB)
TECkit_x86_la_DEPENDENCIES = $(ZLIBDEP)
TECkit_Compiler_x86_la_CFLAGS = $(ZLIB_CFLAGS)
TECkit_Compiler_x86_la_SOURCES = @top_builddir@/source/Compiler.cpp $(ZLIB_SOURCES)
TECkit_Compiler_x86_la_SOURCES += @top_builddir@/source/UnicodeNames.cpp
TECkit_Compiler_x86_la_LDFLAGS = $(LDFLAGS) $(OLD_LIB_LDFLAGS) -no-undefined $(LDZLIB)
TECkit_Compiler_x86_la_DEPENDENCIES = $(ZLIBDEP)
else
libTECkit_la_CFLAGS = $(ZLIB_CFLAGS)
libTECkit_la_SOURCES = @top_builddir@/source/Engine.cpp $(ZLIB_SOURCES)
libTECkit_la_LDFLAGS = $(LDFLAGS) -no-undefined $(LDZLIB)
libTECkit_la_DEPENDENCIES = $(ZLIBDEP)
libTECkit_Compiler_la_CFLAGS = $(ZLIB_CFLAGS)
libTECkit_Compiler_la_SOURCES = @top_builddir@/source/Compiler.cpp $(ZLIB_SOURCES)
libTECkit_Compiler_la_SOURCES += @top_builddir@/source/UnicodeNames.cpp
libTECkit_Compiler_la_LDFLAGS = $(LDFLAGS) -no-undefined $(LDZLIB)
libTECkit_Compiler_la_DEPENDENCIES = $(ZLIBDEP)
endif

if OLD_LIB_NAMES
# --> RC Support, only for the Windows libs
TECkit_x86_la_DEPENDENCIES += Engine_ver.o
TECkit_x86_la_LDFLAGS      += -Wl,Engine_ver.o
Engine_ver.o: $(teckitdir)/Engine_ver.rc
	$(RC) $(AM_RCFLAGS) $(RCFLAGS) -o $@ $<

TECkit_Compiler_x86_la_DEPENDENCIES += Compiler_ver.o
TECkit_Compiler_x86_la_LDFLAGS      += -Wl,Compiler_ver.o
Compiler_ver.o: $(teckitdir)/Compiler_ver.rc
	$(RC) $(AM_RCFLAGS) $(RCFLAGS) -o $@ $<
# <-- RC Support	
endif


noinst_HEADERS = $(teckitdir)/Compiler.h
noinst_HEADERS += $(teckitdir)/Engine.h
noinst_HEADERS += $(teckitdir)/ulong_chartraits.h
noinst_HEADERS += $(teckitdir)/TECkit_Format.h

EXTRA_DIST = $(teckitdir)/NormalizationData.c \
	$(teckitdir)/Perl/TECkit.xs \
	$(teckitdir)/Perl/typemap \
	$(teckitdir)/Perl/lib/Encode/TECkit.pm \
	$(teckitdir)/Perl/Makefile.PL \
	$(teckitdir)/Perl/test.pl

include $(top_srcdir)/source/Public-headers/Makefile.am

