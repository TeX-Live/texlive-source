$(objdir)/pt1.obj: \
	winport.h \
	$(gnuw32dir)/win32lib.h \
	ttf.h \
	pt1.h \
	global.h

$(objdir)/ft.obj: \
	$(freetype2dir)/include/freetype/freetype.h \
	$(freetype2dir)/include/ft2build.h \
	$(freetype2dir)/include/freetype/config/ftheader.h \
	$(freetype2dir)/include/freetype/internal/internal.h \
	$(freetype2dir)/include/freetype/config/ftconfig.h \
	$(freetype2dir)/include/freetype/config/ftoption.h \
	$(freetype2dir)/include/freetype/fterrors.h \
	$(freetype2dir)/include/freetype/ftmoderr.h \
	$(freetype2dir)/include/freetype/fttypes.h \
	$(freetype2dir)/include/freetype/ftsystem.h \
	$(freetype2dir)/include/freetype/ftimage.h \
	$(freetype2dir)/include/freetype/ftglyph.h \
	$(freetype2dir)/include/freetype/freetype.h \
	$(freetype2dir)/include/freetype/ftsnames.h \
	$(freetype2dir)/include/freetype/ttnameid.h \
	$(freetype2dir)/include/freetype/ftoutln.h \
	winport.h \
	$(gnuw32dir)/win32lib.h \
	pt1.h \
	global.h

$(objdir)/ttf2pt1.obj: \
	winport.h \
	$(gnuw32dir)/win32lib.h \
	pt1.h \
	global.h \
	version.h

$(objdir)/ttf.obj: \
	winport.h \
	$(gnuw32dir)/win32lib.h \
	ttf.h \
	pt1.h \
	global.h

$(objdir)/t1asm.obj: \
	

$(objdir)/bitmap.obj: \
	pt1.h \
	global.h

$(objdir)/bdf.obj: \
	winport.h \
	$(gnuw32dir)/win32lib.h \
	pt1.h \
	global.h

$(objdir)/runt1asm.obj: \
	t1asm.c

