AUTOMAKE_OPTIONS = 1.4 foreign
AUTOTOOL_VERSION=autoconf-2.52 automake-1.5 libtool-1.4.2
DEFAULT_INCLUDES = # nothing - no default of -I. -I$(srcdir)
DEFS = @DEFS@ -I$(top_builddir) -I$(top_srcdir) # also for automake 1.4

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

EXTRA_DIST = $(aclocal_DATA) unzzip.h
CLEANFILES = $(WCC10_CLEAN)

zzip_LDADD = ../zzip/libzzip.la @RESOLVES@ -lz
zziptest_LDADD = ../zzip/libzzip.la @RESOLVES@ -lz
zzcat_LDADD = ../zzip/libzzip.la @RESOLVES@ -lz
zzdir_LDADD = ../zzip/libzzip.la @RESOLVES@ -lz
zzxorcat_LDADD = ../zzip/libzzip.la @RESOLVES@ -lz
zzxordir_LDADD = ../zzip/libzzip.la @RESOLVES@ -lz
zzobfuscated_LDADD = ../zzip/libzzip.la @RESOLVES@ -lz

unzzip_LDADD = ../zzip/libzzip.la @RESOLVES@ -lz
unzzip_SOURCES = unzzip.c unzzipcat-zip.c unzzipdir-zip.c
unzzip_mix_LDADD = ../zzip/libzzip.la @RESOLVES@ -lz
unzzip_mix_SOURCES = unzzip.c unzzipcat-mix.c unzzipdir-mix.c
unzzip_mem_LDADD = ../zzip/libzzipmmapped.la @RESOLVES@ -lz
unzzip_mem_SOURCES = unzzip.c unzzipcat-mem.c unzzipdir-mem.c
unzzip_big_LDADD = ../zzip/libzzipfseeko.la @RESOLVES@ -lz
unzzip_big_SOURCES = unzzip.c unzzipcat-big.c unzzipdir-big.c

unzip_mem_LDADD = ../zzip/libzzipmmapped.la @RESOLVES@ -lz

../zzip/libzzip.la : @top_srcdir@/zzip/*.c
	(cd ../zzip && $(MAKE) `basename $@`)
