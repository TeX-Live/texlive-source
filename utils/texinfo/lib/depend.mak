$(objdir)/substring.obj: \
	system.h \
	../config.h \
	$(gnuw32dir)/win32lib.h \
	gettext.h

$(objdir)/xexit.obj: \
	system.h \
	../config.h \
	$(gnuw32dir)/win32lib.h \
	gettext.h

$(objdir)/strdup.obj: \
	../config.h

$(objdir)/getopt.obj: \
	../config.h \
	gettext.h \
	getopt.h

$(objdir)/xstrdup.obj: \
	xalloc.h

$(objdir)/getopt1.obj: \
	../config.h \
	getopt.h

$(objdir)/strncasecmp.obj: \
	strcasecmp.c

$(objdir)/tempname.obj: \
	

$(objdir)/alloca.obj: \
	../config.h \
	xalloc.h

$(objdir)/strcasecmp.obj: \
	

$(objdir)/mkstemp.obj: \
	../config.h

