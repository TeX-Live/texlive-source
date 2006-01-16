$(objdir)/gmempp.obj: \
	../aconf.h \
	../aconf2.h \
	gmem.h

$(objdir)/vms_directory.obj: \
	vms_dirent.h \
	$(kpathseadir)/types.h \
	vms_sys_dirent.h

$(objdir)/vms_unlink.obj: \
	

$(objdir)/gfile.obj: \
	../aconf.h \
	../aconf2.h \
	GString.h \
	gfile.h \
	gtypes.h

$(objdir)/GString.obj: \
	../aconf.h \
	../aconf2.h \
	gtypes.h \
	GString.h

$(objdir)/GHash.obj: \
	../aconf.h \
	../aconf2.h \
	gmem.h \
	GString.h \
	GHash.h \
	gtypes.h

$(objdir)/GList.obj: \
	../aconf.h \
	../aconf2.h \
	gmem.h \
	GList.h \
	gtypes.h

$(objdir)/gmem.obj: \
	../aconf.h \
	../aconf2.h \
	gmem.h

$(objdir)/parseargs.obj: \
	parseargs.h \
	gtypes.h

$(objdir)/vms_unix_times.obj: \
	vms_unix_time.h

