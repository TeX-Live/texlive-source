teckitdir = $(top_srcdir)/source

ZLIBSRCDIR = @ZLIBSRCDIR@

if SYSTEM_ZLIB
ZLIB_CFLAGS = @ZLIBCPPFLAGS@
ZLIB_SOURCES =
else
ZLIB_CFLAGS = -I$(ZLIBSRCDIR)
ZLIB_SOURCES  = $(ZLIBSRCDIR)/adler32.c
ZLIB_SOURCES += $(ZLIBSRCDIR)/compress.c
ZLIB_SOURCES += $(ZLIBSRCDIR)/crc32.c
ZLIB_SOURCES += $(ZLIBSRCDIR)/deflate.c
ZLIB_SOURCES += $(ZLIBSRCDIR)/infback.c
ZLIB_SOURCES += $(ZLIBSRCDIR)/inffast.c
ZLIB_SOURCES += $(ZLIBSRCDIR)/inflate.c
ZLIB_SOURCES += $(ZLIBSRCDIR)/inftrees.c
ZLIB_SOURCES += $(ZLIBSRCDIR)/trees.c
ZLIB_SOURCES += $(ZLIBSRCDIR)/uncompr.c
ZLIB_SOURCES += $(ZLIBSRCDIR)/zutil.c
endif

AM_CPPFLAGS  += $(ZLIBCPPFLAGS)
AM_CFLAGS    = $(AM_CPPFLAGS)

if OLD_LIB_NAMES
OLD_LIB_LDFLAGS = -module -avoid-version -Wl,--add-stdcall-alias
TECkit_x86_la_CFLAGS = $(ZLIB_CFLAGS)
TECkit_x86_la_SOURCES = $(teckitdir)/Engine.cpp $(teckitdir)/Engine_ver.rc $(ZLIB_SOURCES)
TECkit_x86_la_LDFLAGS = $(LDFLAGS) $(OLD_LIB_LDFLAGS) -no-undefined $(LDZLIB)
TECkit_x86_la_DEPENDENCIES = $(ZLIBDEP)
TECkit_Compiler_x86_la_CFLAGS = $(ZLIB_CFLAGS)
TECkit_Compiler_x86_la_SOURCES = $(teckitdir)/Compiler.cpp $(ZLIB_SOURCES)
TECkit_Compiler_x86_la_SOURCES += $(teckitdir)/UnicodeNames.cpp
TECkit_Compiler_x86_la_LDFLAGS = $(LDFLAGS) $(OLD_LIB_LDFLAGS) -no-undefined $(LDZLIB)
TECkit_Compiler_x86_la_DEPENDENCIES = $(ZLIBDEP)
else
libTECkit_la_CFLAGS = $(ZLIB_CFLAGS)
libTECkit_la_SOURCES = $(teckitdir)/Engine.cpp $(ZLIB_SOURCES)
libTECkit_la_LDFLAGS = $(LDFLAGS) -no-undefined $(LDZLIB)
libTECkit_la_DEPENDENCIES = $(ZLIBDEP)
libTECkit_Compiler_la_CFLAGS = $(ZLIB_CFLAGS)
libTECkit_Compiler_la_SOURCES = $(teckitdir)/Compiler.cpp $(ZLIB_SOURCES)
libTECkit_Compiler_la_SOURCES += $(teckitdir)/UnicodeNames.cpp
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

