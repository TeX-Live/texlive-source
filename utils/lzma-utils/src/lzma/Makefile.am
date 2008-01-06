AM_CPPFLAGS = @SDK_CXXFLAGS@ -I@top_srcdir@/src/sdk -I@top_srcdir@/src/sdk/7zip

# Avoid dependency on libstdc++:
#AM_LDFLAGS = -lsupc++ -static-libgcc

bin_PROGRAMS = lzma

lzma_SOURCES = lzmp.cpp Exception.h \
		@top_srcdir@/src/sdk/Common/C_FileIO.cpp \
		@top_srcdir@/src/sdk/Common/CRC.cpp \
		@top_srcdir@/src/sdk/Common/Alloc.cpp \
		@top_srcdir@/src/sdk/7zip/Common/FileStreams.cpp \
		@top_srcdir@/src/sdk/7zip/Common/InBuffer.cpp \
		@top_srcdir@/src/sdk/7zip/Common/OutBuffer.cpp

lzma_LDADD = @top_builddir@/src/sdk/7zip/Compress/LZMA/libLZMA.a \
		@top_builddir@/src/sdk/7zip/Compress/LZ/libLZ.a \
		@top_builddir@/src/sdk/7zip/Common/libCommon.a \
		@top_builddir@/src/sdk/7zip/Compress/RangeCoder/libRangeCoder.a

dist_man_MANS = lzma.1

# Create symlinks for unlzma and lzcat:
install-exec-hook:
	cd $(DESTDIR)$(bindir) && \
	rm -f unlzma lzcat && \
	$(LN_S) lzma unlzma && \
	$(LN_S) lzma lzcat

install-data-hook:
	cd $(DESTDIR)$(mandir)/man1 && \
	rm -f unlzma.1 lzcat.1 && \
	$(LN_S) lzma.1 unlzma.1 && \
	$(LN_S) lzma.1 lzcat.1

uninstall-hook:
	cd $(DESTDIR)$(bindir) && \
	rm -f unlzma lzcat
	cd $(DESTDIR)$(mandir)/man1 && \
	rm -f unlzma.1 lzcat.1

# For getopt_long() replacement:
lzma_DEPENDENCIES = $(LIBOBJS)
lzma_LDADD += $(LIBOBJS)

EXTRA_DIST = gettext.h getopt_.h getopt.c getopt1.c getopt_int.h
BUILT_SOURCES = $(GETOPT_H)
MOSTLYCLEANFILES = getopt.h getopt.h-t

getopt.h: getopt_.h
	{ echo '/* DO NOT EDIT! GENERATED AUTOMATICALLY! */'; \
	  cat $(srcdir)/getopt_.h; \
	} > $@-t
	mv -f $@-t $@
