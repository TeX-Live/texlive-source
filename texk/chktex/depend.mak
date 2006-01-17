$(objdir)/FindErrs.obj: \
	ChkTeX.h \
	$(gnuw32dir)/win32lib.h \
	config.h \
	chktypes.h \
	FindErrs.h \
	OpSys.h \
	Utility.h \
	Resource.h

$(objdir)/chktex.obj: \
	getopt.h \
	ChkTeX.h \
	$(gnuw32dir)/win32lib.h \
	config.h \
	chktypes.h \
	FindErrs.h \
	OpSys.h \
	Utility.h \
	Resource.h \
	$(kpathseadir)/config.h \
	$(kpathseadir)/c-auto.h \
	$(kpathseadir)/c-std.h \
	$(kpathseadir)/c-unistd.h \
	$(kpathseadir)/systypes.h \
	$(kpathseadir)/c-memstr.h \
	$(kpathseadir)/c-errno.h \
	$(kpathseadir)/c-minmax.h \
	$(kpathseadir)/c-limits.h \
	$(kpathseadir)/c-proto.h \
	$(kpathseadir)/debug.h \
	$(kpathseadir)/types.h \
	$(kpathseadir)/lib.h \
	$(kpathseadir)/progname.h

$(objdir)/Utility.obj: \
	ChkTeX.h \
	$(gnuw32dir)/win32lib.h \
	config.h \
	chktypes.h \
	FindErrs.h \
	Utility.h \
	Resource.h \
	OpSys.h

$(objdir)/getopt1.obj: \
	config.h \
	getopt.h

$(objdir)/OpSys.obj: \
	$(kpathseadir)/config.h \
	$(kpathseadir)/c-auto.h \
	$(kpathseadir)/c-std.h \
	$(kpathseadir)/c-unistd.h \
	$(kpathseadir)/systypes.h \
	$(kpathseadir)/c-memstr.h \
	$(kpathseadir)/c-errno.h \
	$(kpathseadir)/c-minmax.h \
	$(kpathseadir)/c-limits.h \
	$(kpathseadir)/c-proto.h \
	$(gnuw32dir)/win32lib.h \
	$(kpathseadir)/debug.h \
	$(kpathseadir)/types.h \
	$(kpathseadir)/lib.h \
	$(kpathseadir)/progname.h \
	$(kpathseadir)/expand.h \
	$(kpathseadir)/tex-file.h \
	$(kpathseadir)/c-vararg.h \
	ChkTeX.h \
	config.h \
	chktypes.h \
	FindErrs.h \
	OpSys.h \
	Utility.h

$(objdir)/getopt.obj: \
	config.h

$(objdir)/Resource.obj: \
	ChkTeX.h \
	$(gnuw32dir)/win32lib.h \
	config.h \
	chktypes.h \
	FindErrs.h \
	OpSys.h \
	Utility.h \
	Resource.h

