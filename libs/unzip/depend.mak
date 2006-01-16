# DO NOT DELETE

$(objdir)/api.obj: api.c  \
	unzip.h \
	windll/structs.h unzpriv.h \
	$(gnuw32dir)/win32lib.h \
	$(gnuw32dir)/oldnames.h win32/w32cfg.h \
	globals.h version.h windll/windll.h \
	windll/structs.h windll/decs.h
$(objdir)/crc32.obj: crc32.c zip.h unzip.h \
	windll/structs.h unzpriv.h \
	$(gnuw32dir)/win32lib.h \
	$(gnuw32dir)/oldnames.h win32/w32cfg.h \
	globals.h
$(objdir)/crctab.obj: crctab.c zip.h unzip.h \
	windll/structs.h unzpriv.h \
	$(gnuw32dir)/win32lib.h \
	$(gnuw32dir)/oldnames.h win32/w32cfg.h \
	globals.h
$(objdir)/explode.obj: explode.c unzip.h \
	windll/structs.h unzpriv.h \
	$(gnuw32dir)/win32lib.h \
	$(gnuw32dir)/oldnames.h win32/w32cfg.h \
	globals.h
$(objdir)/extract.obj: extract.c unzip.h \
	windll/structs.h unzpriv.h \
	$(gnuw32dir)/win32lib.h \
	$(gnuw32dir)/oldnames.h win32/w32cfg.h \
	globals.h crypt.h windll/windll.h \
	windll/structs.h windll/decs.h
$(objdir)/fileio.obj: fileio.c unzip.h \
	windll/structs.h unzpriv.h \
	$(gnuw32dir)/win32lib.h \
	$(gnuw32dir)/oldnames.h win32/w32cfg.h \
	globals.h windll/windll.h \
	windll/structs.h windll/decs.h crypt.h ttyio.h ebcdic.h
$(objdir)/globals.obj: globals.c unzip.h \
	windll/structs.h unzpriv.h \
	$(gnuw32dir)/win32lib.h \
	$(gnuw32dir)/oldnames.h win32/w32cfg.h \
	globals.h
$(objdir)/inflate.obj: inflate.c inflate.h unzip.h \
	windll/structs.h unzpriv.h \
	$(gnuw32dir)/win32lib.h \
	$(gnuw32dir)/oldnames.h win32/w32cfg.h \
	globals.h
$(objdir)/list.obj: list.c unzip.h \
	windll/structs.h unzpriv.h \
	$(gnuw32dir)/win32lib.h \
	$(gnuw32dir)/oldnames.h win32/w32cfg.h \
	globals.h windll/windll.h \
	windll/structs.h windll/decs.h
$(objdir)/match.obj: match.c unzip.h \
	windll/structs.h unzpriv.h \
	$(gnuw32dir)/win32lib.h \
	$(gnuw32dir)/oldnames.h win32/w32cfg.h \
	globals.h
$(objdir)/process.obj: process.c unzip.h \
	windll/structs.h unzpriv.h \
	$(gnuw32dir)/win32lib.h \
	$(gnuw32dir)/oldnames.h win32/w32cfg.h \
	globals.h windll/windll.h \
	windll/structs.h windll/decs.h
$(objdir)/ttyio.obj: ttyio.c zip.h unzip.h \
	windll/structs.h unzpriv.h \
	$(gnuw32dir)/win32lib.h \
	$(gnuw32dir)/oldnames.h win32/w32cfg.h \
	globals.h crypt.h ttyio.h
$(objdir)/unreduce.obj: unreduce.c unzip.h \
	windll/structs.h unzpriv.h \
	$(gnuw32dir)/win32lib.h \
	$(gnuw32dir)/oldnames.h win32/w32cfg.h \
	globals.h
$(objdir)/unshrink.obj: unshrink.c unzip.h \
	windll/structs.h unzpriv.h \
	$(gnuw32dir)/win32lib.h \
	$(gnuw32dir)/oldnames.h win32/w32cfg.h \
	globals.h
$(objdir)/zipinfo.obj: zipinfo.c unzip.h \
	windll/structs.h unzpriv.h \
	$(gnuw32dir)/win32lib.h \
	$(gnuw32dir)/oldnames.h win32/w32cfg.h \
	globals.h
