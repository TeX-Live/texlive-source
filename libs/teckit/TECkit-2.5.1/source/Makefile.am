teckitdir = $(top_srcdir)/source

zlibdir = $(top_srcdir)/zlib-1.2.3

if SYSTEM_ZLIB
ZLIB_CFLAGS = 
ZLIB_SOURCES =
else
ZLIB_CFLAGS = -I$(zlibdir)
ZLIB_SOURCES  = $(zlibdir)/adler32.c
ZLIB_SOURCES += $(zlibdir)/compress.c
ZLIB_SOURCES += $(zlibdir)/crc32.c
ZLIB_SOURCES += $(zlibdir)/deflate.c
ZLIB_SOURCES += $(zlibdir)/infback.c
ZLIB_SOURCES += $(zlibdir)/inffast.c
ZLIB_SOURCES += $(zlibdir)/inflate.c
ZLIB_SOURCES += $(zlibdir)/inftrees.c
ZLIB_SOURCES += $(zlibdir)/trees.c
ZLIB_SOURCES += $(zlibdir)/uncompr.c
ZLIB_SOURCES += $(zlibdir)/zutil.c
endif

AM_CPPFLAGS  += $(ZLIB_CFLAGS)
AM_CFLAGS    = $(AM_CPPFLAGS)

if OLD_LIB_NAMES
OLD_LIB_LDFLAGS = -module -avoid-version -Wl,--add-stdcall-alias
TECkit_x86_la_SOURCES = $(teckitdir)/Engine.cpp $(teckitdir)/Engine_ver.rc $(ZLIB_SOURCES)
TECkit_x86_la_LDFLAGS = $(LDFLAGS) $(OLD_LIB_LDFLAGS) -no-undefined
TECkit_Compiler_x86_la_SOURCES = $(teckitdir)/Compiler.cpp $(ZLIB_SOURCES)
TECkit_Compiler_x86_la_SOURCES += $(teckitdir)/UnicodeNames.cpp
TECkit_Compiler_x86_la_LDFLAGS = $(LDFLAGS) $(OLD_LIB_LDFLAGS) -no-undefined
else
libTECkit_la_SOURCES = $(teckitdir)/Engine.cpp $(ZLIB_SOURCES)
libTECkit_la_LDFLAGS = $(LDFLAGS) -no-undefined
libTECkit_Compiler_la_SOURCES = $(teckitdir)/Compiler.cpp $(ZLIB_SOURCES)
libTECkit_Compiler_la_SOURCES += $(teckitdir)/UnicodeNames.cpp
libTECkit_Compiler_la_LDFLAGS = $(LDFLAGS) -no-undefined
endif

if OLD_LIB_NAMES
# --> RC Support, only for the Windows libs
TECkit_x86_la_DEPENDENCIES = Engine_ver.o
TECkit_x86_la_LDFLAGS      += -Wl,Engine_ver.o
Engine_ver.o: $(teckitdir)/Engine_ver.rc
	$(RC) $(AM_RCFLAGS) $(RCFLAGS) -o $@ $<

TECkit_Compiler_x86_la_DEPENDENCIES = Compiler_ver.o
TECkit_Compiler_x86_la_LDFLAGS      += -Wl,Compiler_ver.o
Compiler_ver.o: $(teckitdir)/Compiler_ver.rc
	$(RC) $(AM_RCFLAGS) $(RCFLAGS) -o $@ $<
# <-- RC Support	
endif


noinst_HEADERS = $(teckitdir)/Compiler.h
noinst_HEADERS += $(teckitdir)/Engine.h
noinst_HEADERS += $(teckitdir)/ulong_chartraits.h
noinst_HEADERS += $(teckitdir)/TECkit_Format.h

EXTRA_DIST = $(teckitdir)/NormalizationData.c

include $(top_srcdir)/source/Public-headers/Makefile.am
