$(objdir)/writeout.obj: \
	$(curldir)/include/curl/curl.h \
	$(curldir)/include/curl/types.h \
	$(curldir)/include/curl/easy.h \
	$(curldir)/include/curl/mprintf.h \
	writeout.h

$(objdir)/hugehelp.obj: \
	

$(objdir)/urlglob.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h \
	$(curldir)/include/curl/curl.h \
	$(curldir)/include/curl/types.h \
	$(curldir)/include/curl/easy.h \
	urlglob.h

$(objdir)/main.obj: \
	setup.h \
	config-win32.h \
	$(gnuw32dir)/win32lib.h \
	$(curldir)/include/curl/curl.h \
	$(curldir)/include/curl/types.h \
	$(curldir)/include/curl/easy.h \
	$(curldir)/include/curl/mprintf.h \
	urlglob.h \
	writeout.h \
	version.h

