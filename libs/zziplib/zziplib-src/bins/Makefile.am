AUTOMAKE_OPTIONS = 1.4 foreign
AUTOTOOL_VERSION=autoconf-2.52 automake-1.5 libtool-1.4.2
DEFAULT_INCLUDES = # nothing - no default of -I. -I$(srcdir)
DEFS = @DEFS@ -I$(top_builddir) -I$(top_srcdir) # also for automake 1.4
CFLAGS = @CFLAGS@ @ASAN_CFLAGS@

bin_PROGRAMS = zzcat zzdir zzxorcat zzxordir zzxorcopy \
               unzzip unzzip-mix unzzip-mem unzzip-big unzip-mem
noinst_PROGRAMS = zziptest zzobfuscated zzip
aclocaldir = $(datadir)/aclocal
aclocal_DATA = zziplib.m4
#
VERSION_INFO=@VERSION_INFO@
RELEASE_INFO=@RELEASE_INFO@
THREAD_SAFE=@THREAD_SAFE@
#

WCC10_CLEAN = *.obj *.lk1 *.mk1 *.mk *.sym *.tgt

noinst_HEADERS = unzzipdir-zip.h unzzipcat-zip.h unzzip-states.h zzipmake-zip.h
EXTRA_DIST = $(aclocal_DATA) $(noinst_HEADERS)
CLEANFILES = $(WCC10_CLEAN)

zziptest_LDADD = ../zzip/libzzip.la @RESOLVES@ -lz @ASAN_LIBS@
zzcat_LDADD = ../zzip/libzzip.la @RESOLVES@ -lz @ASAN_LIBS@
zzdir_LDADD = ../zzip/libzzip.la @RESOLVES@ -lz @ASAN_LIBS@
zzxorcat_LDADD = ../zzip/libzzip.la @RESOLVES@ -lz @ASAN_LIBS@
zzxordir_LDADD = ../zzip/libzzip.la @RESOLVES@ -lz @ASAN_LIBS@
zzobfuscated_LDADD = ../zzip/libzzip.la @RESOLVES@ -lz @ASAN_LIBS@

zzip_LDADD = ../zzip/libzzip.la @RESOLVES@ -lz @ASAN_LIBS@
zzip_SOURCES = zzip.c zzipmake-zip.c

unzzip_LDADD = ../zzip/libzzip.la @RESOLVES@ -lz @ASAN_LIBS@
unzzip_SOURCES = unzzip.c unzzipcat-zip.c unzzipdir-zip.c
unzzip_mix_LDADD = ../zzip/libzzip.la @RESOLVES@ -lz @ASAN_LIBS@
unzzip_mix_SOURCES = unzzip.c unzzipcat-mix.c unzzipdir-mix.c
unzzip_mem_LDADD = ../zzip/libzzipmmapped.la @RESOLVES@ -lz @ASAN_LIBS@
unzzip_mem_SOURCES = unzzip.c unzzipcat-mem.c unzzipdir-mem.c
unzzip_big_LDADD = ../zzip/libzzipfseeko.la @RESOLVES@ -lz @ASAN_LIBS@
unzzip_big_SOURCES = unzzip.c unzzipcat-big.c unzzipdir-big.c

unzip_mem_LDADD = ../zzip/libzzipmmapped.la @RESOLVES@ -lz @ASAN_LIBS@

../zzip/libzzip.la : @top_srcdir@/zzip/*.c
	(cd ../zzip && $(MAKE) `basename $@`)
