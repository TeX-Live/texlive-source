$(objdir)/macro.obj: \
	../lib/system.h \
	../config.h \
	$(gnuw32dir)/win32lib.h \
	../lib/gettext.h \
	macro.h \
	makeinfo.h \
	insertion.h \
	files.h \
	cmds.h

$(objdir)/toc.obj: \
	../lib/system.h \
	../config.h \
	$(gnuw32dir)/win32lib.h \
	../lib/gettext.h \
	makeinfo.h \
	insertion.h \
	cmds.h \
	files.h \
	macro.h \
	node.h \
	html.h \
	lang.h \
	sectioning.h \
	toc.h

$(objdir)/files.obj: \
	../lib/system.h \
	../config.h \
	$(gnuw32dir)/win32lib.h \
	../lib/gettext.h \
	files.h \
	macro.h \
	makeinfo.h

$(objdir)/lang.obj: \
	../lib/system.h \
	../config.h \
	$(gnuw32dir)/win32lib.h \
	../lib/gettext.h \
	cmds.h \
	lang.h \
	makeinfo.h \
	sectioning.h \
	insertion.h \
	xml.h

$(objdir)/insertion.obj: \
	../lib/system.h \
	../config.h \
	$(gnuw32dir)/win32lib.h \
	../lib/gettext.h \
	cmds.h \
	defun.h \
	insertion.h \
	macro.h \
	makeinfo.h \
	multi.h \
	xml.h

$(objdir)/index.obj: \
	../lib/system.h \
	../config.h \
	$(gnuw32dir)/win32lib.h \
	../lib/gettext.h \
	index.h \
	makeinfo.h \
	insertion.h \
	cmds.h \
	lang.h \
	macro.h \
	toc.h \
	html.h \
	xml.h

$(objdir)/node.obj: \
	../lib/system.h \
	../config.h \
	$(gnuw32dir)/win32lib.h \
	../lib/gettext.h \
	cmds.h \
	files.h \
	footnote.h \
	macro.h \
	makeinfo.h \
	node.h \
	html.h \
	sectioning.h \
	insertion.h \
	xml.h

$(objdir)/makeinfo.obj: \
	../lib/system.h \
	../config.h \
	$(gnuw32dir)/win32lib.h \
	../lib/gettext.h \
	$(kpathseadir)/getopt.h \
	makeinfo.h \
	cmds.h \
	files.h \
	footnote.h \
	html.h \
	index.h \
	insertion.h \
	macro.h \
	node.h \
	toc.h \
	xml.h

$(objdir)/sectioning.obj: \
	../lib/system.h \
	../config.h \
	$(gnuw32dir)/win32lib.h \
	../lib/gettext.h \
	macro.h \
	makeinfo.h \
	node.h \
	toc.h \
	sectioning.h \
	insertion.h \
	cmds.h \
	html.h \
	xml.h

$(objdir)/cmds.obj: \
	../lib/system.h \
	../config.h \
	$(gnuw32dir)/win32lib.h \
	../lib/gettext.h \
	cmds.h \
	defun.h \
	insertion.h \
	files.h \
	footnote.h \
	index.h \
	makeinfo.h \
	lang.h \
	macro.h \
	node.h \
	sectioning.h \
	toc.h \
	multi.h \
	html.h \
	xml.h

$(objdir)/xml.obj: \
	../lib/system.h \
	../config.h \
	$(gnuw32dir)/win32lib.h \
	../lib/gettext.h \
	makeinfo.h \
	insertion.h \
	macro.h \
	cmds.h \
	lang.h \
	xml.h

$(objdir)/defun.obj: \
	../lib/system.h \
	../config.h \
	$(gnuw32dir)/win32lib.h \
	../lib/gettext.h \
	defun.h \
	insertion.h \
	xml.h \
	makeinfo.h \
	cmds.h \
	html.h

$(objdir)/multi.obj: \
	../lib/system.h \
	../config.h \
	$(gnuw32dir)/win32lib.h \
	../lib/gettext.h \
	multi.h \
	insertion.h \
	makeinfo.h \
	cmds.h \
	xml.h

$(objdir)/html.obj: \
	../lib/system.h \
	../config.h \
	$(gnuw32dir)/win32lib.h \
	../lib/gettext.h \
	cmds.h \
	html.h \
	lang.h \
	makeinfo.h \
	sectioning.h

$(objdir)/footnote.obj: \
	../lib/system.h \
	../config.h \
	$(gnuw32dir)/win32lib.h \
	../lib/gettext.h \
	footnote.h \
	macro.h \
	makeinfo.h \
	node.h \
	xml.h

