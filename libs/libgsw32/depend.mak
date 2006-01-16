$(objdir)/gvwgsver.obj: \
	gvwgsver.h

$(objdir)/ghostscr.obj: \
	$(gnuw32dir)/win32lib.h \
	gs32lib.h \
	../../gstools/ghostscript/src/gsdll.h \
	../../gstools/ghostscript/src/iapi.h \
	../../gstools/ghostscript/src/errors.h \
	gvwgsver.h

