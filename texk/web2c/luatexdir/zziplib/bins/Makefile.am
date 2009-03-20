AUTOMAKE_OPTIONS = 1.4 foreign
AUTOTOOL_VERSION=autoconf-2.52 automake-1.5 libtool-1.4.2
DEFAULT_INCLUDES = # nothing - no default of -I. -I$(srcdir)
DEFS = @DEFS@ -I$(top_builddir) -I$(top_srcdir) # also for automake 1.4

bin_PROGRAMS = zzcat zzdir zzxorcat zzxordir zzxorcopy \
               unzzip unzzipdir unzzipcat unzzipcat-seeko \
               unzzipdir-mem unzzipcat-mem unzip-mem
noinst_PROGRAMS = zziptest zzobfuscated zzip
aclocaldir = $(datadir)/aclocal
aclocal_DATA = zziplib.m4
#
VERSION_INFO=@VERSION_INFO@
RELEASE_INFO=@RELEASE_INFO@
THREAD_SAFE=@THREAD_SAFE@
#

WCC10_CLEAN = *.obj *.lk1 *.mk1 *.mk *.sym *.tgt

EXTRA_DIST = $(aclocal_DATA)
CLEANFILES = $(WCC10_CLEAN)

unzzip_LDFLAGS = @ZZIPLIB_LDFLAGS@
unzzip_LDADD = ../zzip/libzzip.la @RESOLVES@ -lz
zzip_LDFLAGS = @ZZIPLIB_LDFLAGS@
zzip_LDADD = ../zzip/libzzip.la @RESOLVES@ -lz
zziptest_LDFLAGS = @ZZIPLIB_LDFLAGS@
zziptest_LDADD = ../zzip/libzzip.la @RESOLVES@ -lz
zzcat_LDFLAGS =  @ZZIPLIB_LDFLAGS@
zzcat_LDADD = ../zzip/libzzip.la @RESOLVES@ -lz
zzdir_LDFLAGS =  @ZZIPLIB_LDFLAGS@
zzdir_LDADD = ../zzip/libzzip.la @RESOLVES@ -lz
zzxorcat_LDFLAGS =  @ZZIPLIB_LDFLAGS@
zzxorcat_LDADD = ../zzip/libzzip.la @RESOLVES@ -lz
zzxordir_LDFLAGS =  @ZZIPLIB_LDFLAGS@
zzxordir_LDADD = ../zzip/libzzip.la @RESOLVES@ -lz
zzobfuscated_LDFLAGS =  @ZZIPLIB_LDFLAGS@
zzobfuscated_LDADD = ../zzip/libzzip.la @RESOLVES@ -lz

# these use the simplified mmapped disk version
unzzipdir_LDFLAGS = @ZZIPLIB_LDFLAGS@
unzzipdir_LDADD = ../zzip/libzzipmmapped.la @RESOLVES@ -lz
unzzipcat_LDFLAGS = @ZZIPLIB_LDFLAGS@
unzzipcat_LDADD = ../zzip/libzzipmmapped.la @RESOLVES@ -lz
unzzipdir_mem_LDFLAGS = @ZZIPLIB_LDFLAGS@
unzzipdir_mem_LDADD = ../zzip/libzzipmmapped.la @RESOLVES@ -lz
unzzipcat_mem_LDFLAGS = @ZZIPLIB_LDFLAGS@
unzzipcat_mem_LDADD = ../zzip/libzzipmmapped.la @RESOLVES@ -lz
unzip_mem_LDFLAGS = @ZZIPLIB_LDFLAGS@
unzip_mem_LDADD = ../zzip/libzzipmmapped.la @RESOLVES@ -lz
# and this uses the simplified fseeko stdio version
unzzipcat_seeko_LDFLAGS = @ZZIPLIB_LDFLAGS@
unzzipcat_seeko_LDADD = ../zzip/libzzipfseeko.la @RESOLVES@ -lz

../zzip/libzzip.la : @top_srcdir@/zzip/*.c
	(cd ../zzip && $(MAKE) `basename $@`)
