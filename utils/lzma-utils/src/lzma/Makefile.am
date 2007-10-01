AM_CXXFLAGS = @SDK_CXXFLAGS@ -I../ -I../sdk -I../sdk/7zip

# Avoid dependency on libstdc++:
#AM_LDFLAGS = -lsupc++ -static-libgcc

bin_PROGRAMS = lzma

lzma_SOURCES = lzmp.cpp Exception.h \
		../sdk/Common/C_FileIO.cpp \
		../sdk/Common/CRC.cpp \
		../sdk/Common/Alloc.cpp \
		../sdk/7zip/Common/FileStreams.cpp \
		../sdk/7zip/Common/InBuffer.cpp \
		../sdk/7zip/Common/OutBuffer.cpp

lzma_LDADD = ../sdk/7zip/Compress/LZMA/libLZMA.a \
		../sdk/7zip/Compress/LZ/libLZ.a \
		../sdk/7zip/Common/libCommon.a \
		../sdk/7zip/Compress/RangeCoder/libRangeCoder.a

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
