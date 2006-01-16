$(objdir)/infcodes.obj: \
	zutil.h \
	zlib.h \
	zconf.h \
	inftrees.h \
	infblock.h \
	infcodes.h \
	infutil.h \
	inffast.h

$(objdir)/minigzip.obj: \
	$(gnuw32dir)/win32lib.h \
	zlib.h \
	zconf.h

$(objdir)/gzio.obj: \
	zutil.h \
	zlib.h \
	zconf.h

$(objdir)/compress.obj: \
	zlib.h \
	zconf.h

$(objdir)/zutil.obj: \
	zutil.h \
	zlib.h \
	zconf.h

$(objdir)/crc32.obj: \
	zlib.h \
	zconf.h

$(objdir)/uncompr.obj: \
	zlib.h \
	zconf.h

$(objdir)/adler32.obj: \
	zlib.h \
	zconf.h

$(objdir)/example.obj: \
	zlib.h \
	zconf.h

$(objdir)/deflate.obj: \
	deflate.h \
	zutil.h \
	zlib.h \
	zconf.h

$(objdir)/trees.obj: \
	deflate.h \
	zutil.h \
	zlib.h \
	zconf.h \
	trees.h

$(objdir)/infutil.obj: \
	zutil.h \
	zlib.h \
	zconf.h \
	infblock.h \
	inftrees.h \
	infcodes.h \
	infutil.h

$(objdir)/inftrees.obj: \
	zutil.h \
	zlib.h \
	zconf.h \
	inftrees.h \
	inffixed.h

$(objdir)/inffast.obj: \
	zutil.h \
	zlib.h \
	zconf.h \
	inftrees.h \
	infblock.h \
	infcodes.h \
	infutil.h \
	inffast.h

$(objdir)/maketree.obj: \
	zutil.h \
	zlib.h \
	zconf.h \
	inftrees.h

$(objdir)/inflate.obj: \
	zutil.h \
	zlib.h \
	zconf.h \
	infblock.h

$(objdir)/infblock.obj: \
	zutil.h \
	zlib.h \
	zconf.h \
	infblock.h \
	inftrees.h \
	infcodes.h \
	infutil.h

