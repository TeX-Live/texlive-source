# DO NOT DELETE

$(objdir)/xmlparse.obj: xmlparse.c winconfig.h \
	 expat.h \
	xmltok.h xmlrole.h
$(objdir)/xmlrole.obj: xmlrole.c winconfig.h \
	xmlrole.h xmltok.h ascii.h
$(objdir)/xmltok.obj: xmltok.c winconfig.h \
	xmltok.h nametab.h xmltok_impl.h \
	 ascii.h \
	xmltok_impl.c asciitab.h utf8tab.h iasciitab.h latin1tab.h xmltok_ns.c
