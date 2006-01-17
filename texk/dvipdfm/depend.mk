ascii85.o: ascii85.c 
colorsp.o: colorsp.c \
  system.h \
  $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h \
  $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h \
  $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/absolute.h $(kpathsea_srcdir)/c-ctype.h \
  $(kpathsea_srcdir)/c-dir.h \
  $(kpathsea_srcdir)/c-fopen.h \
  $(kpathsea_srcdir)/c-namemx.h $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-pathmx.h \
  $(kpathsea_srcdir)/c-stat.h \
  $(kpathsea_srcdir)/c-vararg.h $(kpathsea_srcdir)/cnf.h $(kpathsea_srcdir)/concatn.h \
  $(kpathsea_srcdir)/db.h $(kpathsea_srcdir)/str-list.h $(kpathsea_srcdir)/default.h \
  $(kpathsea_srcdir)/expand.h $(kpathsea_srcdir)/fn.h $(kpathsea_srcdir)/fontmap.h \
  $(kpathsea_srcdir)/hash.h $(kpathsea_srcdir)/getopt.h $(kpathsea_srcdir)/line.h \
  $(kpathsea_srcdir)/magstep.h $(kpathsea_dir)/paths.h $(kpathsea_srcdir)/pathsearch.h \
  $(kpathsea_srcdir)/str-llist.h $(kpathsea_srcdir)/proginit.h $(kpathsea_srcdir)/readable.h \
  $(kpathsea_srcdir)/tex-file.h $(kpathsea_srcdir)/tex-glyph.h $(kpathsea_srcdir)/tex-hush.h \
  $(kpathsea_srcdir)/tex-make.h $(kpathsea_srcdir)/tilde.h $(kpathsea_srcdir)/truncate.h \
  $(kpathsea_srcdir)/variable.h $(kpathsea_srcdir)/xopendir.h $(kpathsea_srcdir)/xstat.h mem.h \
  pdfdev.h numbers.h pdfobj.h pdfparse.h pdfspecial.h dvipdfm.h
dvi.o: dvi.c \
  system.h $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/kpathsea.h \
  $(kpathsea_srcdir)/config.h $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h \
  $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h \
  $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/absolute.h $(kpathsea_srcdir)/c-ctype.h $(kpathsea_srcdir)/c-dir.h \
  $(kpathsea_srcdir)/c-fopen.h \
  $(kpathsea_srcdir)/c-namemx.h \
  $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-pathmx.h $(kpathsea_srcdir)/c-stat.h \
  $(kpathsea_srcdir)/c-vararg.h \
  $(kpathsea_srcdir)/cnf.h $(kpathsea_srcdir)/concatn.h $(kpathsea_srcdir)/db.h \
  $(kpathsea_srcdir)/str-list.h $(kpathsea_srcdir)/default.h $(kpathsea_srcdir)/expand.h \
  $(kpathsea_srcdir)/fn.h $(kpathsea_srcdir)/fontmap.h $(kpathsea_srcdir)/hash.h \
  $(kpathsea_srcdir)/getopt.h $(kpathsea_srcdir)/line.h $(kpathsea_srcdir)/magstep.h \
  $(kpathsea_dir)/paths.h $(kpathsea_srcdir)/pathsearch.h $(kpathsea_srcdir)/str-llist.h \
  $(kpathsea_srcdir)/proginit.h $(kpathsea_srcdir)/readable.h $(kpathsea_srcdir)/tex-file.h \
  $(kpathsea_srcdir)/tex-glyph.h $(kpathsea_srcdir)/tex-hush.h $(kpathsea_srcdir)/tex-make.h \
  $(kpathsea_srcdir)/tilde.h $(kpathsea_srcdir)/truncate.h $(kpathsea_srcdir)/variable.h \
  $(kpathsea_srcdir)/xopendir.h $(kpathsea_srcdir)/xstat.h mem.h error.h mfileio.h \
  numbers.h dvi.h pdfdev.h pdfobj.h dvicodes.h pdflimits.h pdfdoc.h tfm.h \
  vf.h
dvipdfm.o: dvipdfm.c \
  config.h system.h \
  $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h \
  $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h \
  $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/absolute.h $(kpathsea_srcdir)/c-ctype.h $(kpathsea_srcdir)/c-dir.h \
  $(kpathsea_srcdir)/c-fopen.h \
  $(kpathsea_srcdir)/c-namemx.h \
  $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-pathmx.h $(kpathsea_srcdir)/c-stat.h \
  $(kpathsea_srcdir)/c-vararg.h \
  $(kpathsea_srcdir)/cnf.h $(kpathsea_srcdir)/concatn.h $(kpathsea_srcdir)/db.h \
  $(kpathsea_srcdir)/str-list.h $(kpathsea_srcdir)/default.h $(kpathsea_srcdir)/expand.h \
  $(kpathsea_srcdir)/fn.h $(kpathsea_srcdir)/fontmap.h $(kpathsea_srcdir)/hash.h \
  $(kpathsea_srcdir)/getopt.h $(kpathsea_srcdir)/line.h $(kpathsea_srcdir)/magstep.h \
  $(kpathsea_dir)/paths.h $(kpathsea_srcdir)/pathsearch.h $(kpathsea_srcdir)/str-llist.h \
  $(kpathsea_srcdir)/proginit.h $(kpathsea_srcdir)/readable.h $(kpathsea_srcdir)/tex-file.h \
  $(kpathsea_srcdir)/tex-glyph.h $(kpathsea_srcdir)/tex-hush.h $(kpathsea_srcdir)/tex-make.h \
  $(kpathsea_srcdir)/tilde.h $(kpathsea_srcdir)/truncate.h $(kpathsea_srcdir)/variable.h \
  $(kpathsea_srcdir)/xopendir.h $(kpathsea_srcdir)/xstat.h mem.h mfileio.h numbers.h \
  dvi.h error.h pdfdev.h pdfobj.h pdfdoc.h encodings.h type1.h ttf.h \
  colorsp.h pdfparse.h pdfspecial.h vf.h pkfont.h thumbnail.h psimage.h \
  tfm.h
ebb.o: ebb.c \
  system.h $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/kpathsea.h \
  $(kpathsea_srcdir)/config.h $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h \
  $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/absolute.h $(kpathsea_srcdir)/c-ctype.h \
  $(kpathsea_srcdir)/c-dir.h \
  $(kpathsea_srcdir)/c-fopen.h \
  $(kpathsea_srcdir)/c-namemx.h $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-pathmx.h \
  $(kpathsea_srcdir)/c-stat.h \
  $(kpathsea_srcdir)/c-vararg.h $(kpathsea_srcdir)/cnf.h $(kpathsea_srcdir)/concatn.h \
  $(kpathsea_srcdir)/db.h $(kpathsea_srcdir)/str-list.h $(kpathsea_srcdir)/default.h \
  $(kpathsea_srcdir)/expand.h $(kpathsea_srcdir)/fn.h $(kpathsea_srcdir)/fontmap.h \
  $(kpathsea_srcdir)/hash.h $(kpathsea_srcdir)/getopt.h $(kpathsea_srcdir)/line.h \
  $(kpathsea_srcdir)/magstep.h $(kpathsea_dir)/paths.h $(kpathsea_srcdir)/pathsearch.h \
  $(kpathsea_srcdir)/str-llist.h $(kpathsea_srcdir)/proginit.h $(kpathsea_srcdir)/readable.h \
  $(kpathsea_srcdir)/tex-file.h $(kpathsea_srcdir)/tex-glyph.h $(kpathsea_srcdir)/tex-hush.h \
  $(kpathsea_srcdir)/tex-make.h $(kpathsea_srcdir)/tilde.h $(kpathsea_srcdir)/truncate.h \
  $(kpathsea_srcdir)/variable.h $(kpathsea_srcdir)/xopendir.h $(kpathsea_srcdir)/xstat.h mem.h \
  mfileio.h numbers.h pdfobj.h jpeg.h pdfspecial.h pdfparse.h config.h \
  pngimage.h 
encodings.o: encodings.c \
  pdflimits.h pdfobj.h mem.h \
  error.h system.h $(kpathsea_dir)/c-auto.h \
  $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h \
  $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/absolute.h $(kpathsea_srcdir)/c-ctype.h \
  $(kpathsea_srcdir)/c-dir.h \
  $(kpathsea_srcdir)/c-fopen.h \
  $(kpathsea_srcdir)/c-namemx.h $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-pathmx.h \
  $(kpathsea_srcdir)/c-stat.h \
  $(kpathsea_srcdir)/c-vararg.h $(kpathsea_srcdir)/cnf.h $(kpathsea_srcdir)/concatn.h \
  $(kpathsea_srcdir)/db.h $(kpathsea_srcdir)/str-list.h $(kpathsea_srcdir)/default.h \
  $(kpathsea_srcdir)/expand.h $(kpathsea_srcdir)/fn.h $(kpathsea_srcdir)/fontmap.h \
  $(kpathsea_srcdir)/hash.h $(kpathsea_srcdir)/getopt.h $(kpathsea_srcdir)/line.h \
  $(kpathsea_srcdir)/magstep.h $(kpathsea_dir)/paths.h $(kpathsea_srcdir)/pathsearch.h \
  $(kpathsea_srcdir)/str-llist.h $(kpathsea_srcdir)/proginit.h $(kpathsea_srcdir)/readable.h \
  $(kpathsea_srcdir)/tex-file.h $(kpathsea_srcdir)/tex-glyph.h $(kpathsea_srcdir)/tex-hush.h \
  $(kpathsea_srcdir)/tex-make.h $(kpathsea_srcdir)/tilde.h $(kpathsea_srcdir)/truncate.h \
  $(kpathsea_srcdir)/variable.h $(kpathsea_srcdir)/xopendir.h $(kpathsea_srcdir)/xstat.h \
  mfileio.h numbers.h pdfparse.h encodings.h winansi.h
epdf.o: epdf.c system.h $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/kpathsea.h \
  $(kpathsea_srcdir)/config.h $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/absolute.h $(kpathsea_srcdir)/c-ctype.h \
  $(kpathsea_srcdir)/c-dir.h \
  $(kpathsea_srcdir)/c-fopen.h \
  $(kpathsea_srcdir)/c-namemx.h $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-pathmx.h \
  $(kpathsea_srcdir)/c-stat.h \
  $(kpathsea_srcdir)/c-vararg.h $(kpathsea_srcdir)/cnf.h $(kpathsea_srcdir)/concatn.h \
  $(kpathsea_srcdir)/db.h $(kpathsea_srcdir)/str-list.h $(kpathsea_srcdir)/default.h \
  $(kpathsea_srcdir)/expand.h $(kpathsea_srcdir)/fn.h $(kpathsea_srcdir)/fontmap.h \
  $(kpathsea_srcdir)/hash.h $(kpathsea_srcdir)/getopt.h $(kpathsea_srcdir)/line.h \
  $(kpathsea_srcdir)/magstep.h $(kpathsea_dir)/paths.h $(kpathsea_srcdir)/pathsearch.h \
  $(kpathsea_srcdir)/str-llist.h $(kpathsea_srcdir)/proginit.h $(kpathsea_srcdir)/readable.h \
  $(kpathsea_srcdir)/tex-file.h $(kpathsea_srcdir)/tex-glyph.h $(kpathsea_srcdir)/tex-hush.h \
  $(kpathsea_srcdir)/tex-make.h $(kpathsea_srcdir)/tilde.h $(kpathsea_srcdir)/truncate.h \
  $(kpathsea_srcdir)/variable.h $(kpathsea_srcdir)/xopendir.h $(kpathsea_srcdir)/xstat.h mem.h \
  mfileio.h numbers.h pdfobj.h pdfdoc.h pdfspecial.h epdf.h
htex.o: htex.c \
  system.h \
  $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h \
  $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/absolute.h $(kpathsea_srcdir)/c-ctype.h $(kpathsea_srcdir)/c-dir.h \
  $(kpathsea_srcdir)/c-fopen.h \
  $(kpathsea_srcdir)/c-namemx.h \
  $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-pathmx.h $(kpathsea_srcdir)/c-stat.h \
  $(kpathsea_srcdir)/c-vararg.h \
  $(kpathsea_srcdir)/cnf.h $(kpathsea_srcdir)/concatn.h $(kpathsea_srcdir)/db.h \
  $(kpathsea_srcdir)/str-list.h $(kpathsea_srcdir)/default.h $(kpathsea_srcdir)/expand.h \
  $(kpathsea_srcdir)/fn.h $(kpathsea_srcdir)/fontmap.h $(kpathsea_srcdir)/hash.h \
  $(kpathsea_srcdir)/getopt.h $(kpathsea_srcdir)/line.h $(kpathsea_srcdir)/magstep.h \
  $(kpathsea_dir)/paths.h $(kpathsea_srcdir)/pathsearch.h $(kpathsea_srcdir)/str-llist.h \
  $(kpathsea_srcdir)/proginit.h $(kpathsea_srcdir)/readable.h $(kpathsea_srcdir)/tex-file.h \
  $(kpathsea_srcdir)/tex-glyph.h $(kpathsea_srcdir)/tex-hush.h $(kpathsea_srcdir)/tex-make.h \
  $(kpathsea_srcdir)/tilde.h $(kpathsea_srcdir)/truncate.h $(kpathsea_srcdir)/variable.h \
  $(kpathsea_srcdir)/xopendir.h $(kpathsea_srcdir)/xstat.h mem.h mfileio.h numbers.h \
  dvi.h error.h pdfdev.h pdfobj.h htex.h pdfparse.h pdfdoc.h
jpeg.o: jpeg.c \
  system.h \
  $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h \
  $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/absolute.h $(kpathsea_srcdir)/c-ctype.h \
  $(kpathsea_srcdir)/c-dir.h \
  $(kpathsea_srcdir)/c-fopen.h \
  $(kpathsea_srcdir)/c-namemx.h $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-pathmx.h \
  $(kpathsea_srcdir)/c-stat.h \
  $(kpathsea_srcdir)/c-vararg.h $(kpathsea_srcdir)/cnf.h $(kpathsea_srcdir)/concatn.h \
  $(kpathsea_srcdir)/db.h $(kpathsea_srcdir)/str-list.h $(kpathsea_srcdir)/default.h \
  $(kpathsea_srcdir)/expand.h $(kpathsea_srcdir)/fn.h $(kpathsea_srcdir)/fontmap.h \
  $(kpathsea_srcdir)/hash.h $(kpathsea_srcdir)/getopt.h $(kpathsea_srcdir)/line.h \
  $(kpathsea_srcdir)/magstep.h $(kpathsea_dir)/paths.h $(kpathsea_srcdir)/pathsearch.h \
  $(kpathsea_srcdir)/str-llist.h $(kpathsea_srcdir)/proginit.h $(kpathsea_srcdir)/readable.h \
  $(kpathsea_srcdir)/tex-file.h $(kpathsea_srcdir)/tex-glyph.h $(kpathsea_srcdir)/tex-hush.h \
  $(kpathsea_srcdir)/tex-make.h $(kpathsea_srcdir)/tilde.h $(kpathsea_srcdir)/truncate.h \
  $(kpathsea_srcdir)/variable.h $(kpathsea_srcdir)/xopendir.h $(kpathsea_srcdir)/xstat.h mem.h \
  mfileio.h numbers.h dvi.h error.h pdfdev.h pdfobj.h jpeg.h pdfspecial.h
mem.o: mem.c \
  mem.h
mfileio.o: mfileio.c \
  system.h \
  $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h \
  $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h \
  $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/absolute.h $(kpathsea_srcdir)/c-ctype.h \
  $(kpathsea_srcdir)/c-dir.h \
  $(kpathsea_srcdir)/c-fopen.h \
  $(kpathsea_srcdir)/c-namemx.h $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-pathmx.h \
  $(kpathsea_srcdir)/c-stat.h \
  $(kpathsea_srcdir)/c-vararg.h $(kpathsea_srcdir)/cnf.h $(kpathsea_srcdir)/concatn.h \
  $(kpathsea_srcdir)/db.h $(kpathsea_srcdir)/str-list.h $(kpathsea_srcdir)/default.h \
  $(kpathsea_srcdir)/expand.h $(kpathsea_srcdir)/fn.h $(kpathsea_srcdir)/fontmap.h \
  $(kpathsea_srcdir)/hash.h $(kpathsea_srcdir)/getopt.h $(kpathsea_srcdir)/line.h \
  $(kpathsea_srcdir)/magstep.h $(kpathsea_dir)/paths.h $(kpathsea_srcdir)/pathsearch.h \
  $(kpathsea_srcdir)/str-llist.h $(kpathsea_srcdir)/proginit.h $(kpathsea_srcdir)/readable.h \
  $(kpathsea_srcdir)/tex-file.h $(kpathsea_srcdir)/tex-glyph.h $(kpathsea_srcdir)/tex-hush.h \
  $(kpathsea_srcdir)/tex-make.h $(kpathsea_srcdir)/tilde.h $(kpathsea_srcdir)/truncate.h \
  $(kpathsea_srcdir)/variable.h $(kpathsea_srcdir)/xopendir.h $(kpathsea_srcdir)/xstat.h \
  mfileio.h numbers.h error.h
mpost.o: mpost.c \
  system.h $(kpathsea_dir)/c-auto.h \
  $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/absolute.h $(kpathsea_srcdir)/c-ctype.h $(kpathsea_srcdir)/c-dir.h \
  $(kpathsea_srcdir)/c-fopen.h \
  $(kpathsea_srcdir)/c-namemx.h \
  $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-pathmx.h $(kpathsea_srcdir)/c-stat.h \
  $(kpathsea_srcdir)/c-vararg.h \
  $(kpathsea_srcdir)/cnf.h $(kpathsea_srcdir)/concatn.h $(kpathsea_srcdir)/db.h \
  $(kpathsea_srcdir)/str-list.h $(kpathsea_srcdir)/default.h $(kpathsea_srcdir)/expand.h \
  $(kpathsea_srcdir)/fn.h $(kpathsea_srcdir)/fontmap.h $(kpathsea_srcdir)/hash.h \
  $(kpathsea_srcdir)/getopt.h $(kpathsea_srcdir)/line.h $(kpathsea_srcdir)/magstep.h \
  $(kpathsea_dir)/paths.h $(kpathsea_srcdir)/pathsearch.h $(kpathsea_srcdir)/str-llist.h \
  $(kpathsea_srcdir)/proginit.h $(kpathsea_srcdir)/readable.h $(kpathsea_srcdir)/tex-file.h \
  $(kpathsea_srcdir)/tex-glyph.h $(kpathsea_srcdir)/tex-hush.h $(kpathsea_srcdir)/tex-make.h \
  $(kpathsea_srcdir)/tilde.h $(kpathsea_srcdir)/truncate.h $(kpathsea_srcdir)/variable.h \
  $(kpathsea_srcdir)/xopendir.h $(kpathsea_srcdir)/xstat.h mem.h error.h mfileio.h \
  numbers.h dvi.h pdfdev.h pdfobj.h pdfspecial.h pdfparse.h mpost.h \
  pdflimits.h pdfdoc.h
numbers.o: numbers.c system.h $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/kpathsea.h \
  $(kpathsea_srcdir)/config.h $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/absolute.h $(kpathsea_srcdir)/c-ctype.h \
  $(kpathsea_srcdir)/c-dir.h \
  $(kpathsea_srcdir)/c-fopen.h \
  $(kpathsea_srcdir)/c-namemx.h $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-pathmx.h \
  $(kpathsea_srcdir)/c-stat.h \
  $(kpathsea_srcdir)/c-vararg.h $(kpathsea_srcdir)/cnf.h $(kpathsea_srcdir)/concatn.h \
  $(kpathsea_srcdir)/db.h $(kpathsea_srcdir)/str-list.h $(kpathsea_srcdir)/default.h \
  $(kpathsea_srcdir)/expand.h $(kpathsea_srcdir)/fn.h $(kpathsea_srcdir)/fontmap.h \
  $(kpathsea_srcdir)/hash.h $(kpathsea_srcdir)/getopt.h $(kpathsea_srcdir)/line.h \
  $(kpathsea_srcdir)/magstep.h $(kpathsea_dir)/paths.h $(kpathsea_srcdir)/pathsearch.h \
  $(kpathsea_srcdir)/str-llist.h $(kpathsea_srcdir)/proginit.h $(kpathsea_srcdir)/readable.h \
  $(kpathsea_srcdir)/tex-file.h $(kpathsea_srcdir)/tex-glyph.h $(kpathsea_srcdir)/tex-hush.h \
  $(kpathsea_srcdir)/tex-make.h $(kpathsea_srcdir)/tilde.h $(kpathsea_srcdir)/truncate.h \
  $(kpathsea_srcdir)/variable.h $(kpathsea_srcdir)/xopendir.h $(kpathsea_srcdir)/xstat.h \
  error.h mfileio.h numbers.h
pdfdev.o: pdfdev.c config.h \
  system.h $(kpathsea_dir)/c-auto.h \
  $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/absolute.h $(kpathsea_srcdir)/c-ctype.h $(kpathsea_srcdir)/c-dir.h \
  $(kpathsea_srcdir)/c-fopen.h \
  $(kpathsea_srcdir)/c-namemx.h \
  $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-pathmx.h $(kpathsea_srcdir)/c-stat.h \
  $(kpathsea_srcdir)/c-vararg.h \
  $(kpathsea_srcdir)/cnf.h $(kpathsea_srcdir)/concatn.h $(kpathsea_srcdir)/db.h \
  $(kpathsea_srcdir)/str-list.h $(kpathsea_srcdir)/default.h $(kpathsea_srcdir)/expand.h \
  $(kpathsea_srcdir)/fn.h $(kpathsea_srcdir)/fontmap.h $(kpathsea_srcdir)/hash.h \
  $(kpathsea_srcdir)/getopt.h $(kpathsea_srcdir)/line.h $(kpathsea_srcdir)/magstep.h \
  $(kpathsea_dir)/paths.h $(kpathsea_srcdir)/pathsearch.h $(kpathsea_srcdir)/str-llist.h \
  $(kpathsea_srcdir)/proginit.h $(kpathsea_srcdir)/readable.h $(kpathsea_srcdir)/tex-file.h \
  $(kpathsea_srcdir)/tex-glyph.h $(kpathsea_srcdir)/tex-hush.h $(kpathsea_srcdir)/tex-make.h \
  $(kpathsea_srcdir)/tilde.h $(kpathsea_srcdir)/truncate.h $(kpathsea_srcdir)/variable.h \
  $(kpathsea_srcdir)/xopendir.h $(kpathsea_srcdir)/xstat.h mem.h error.h mfileio.h \
  numbers.h dvi.h pdfdev.h pdfobj.h tfm.h pdfdoc.h type1.h ttf.h pkfont.h \
  pdfspecial.h pdfparse.h tpic.h htex.h mpost.h psspecial.h colorsp.h \
  pdflimits.h twiddle.h encodings.h colors.h
pdfdoc.o: pdfdoc.c \
  system.h \
  $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h \
  $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h \
  $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/absolute.h $(kpathsea_srcdir)/c-ctype.h \
  $(kpathsea_srcdir)/c-dir.h \
  $(kpathsea_srcdir)/c-fopen.h \
  $(kpathsea_srcdir)/c-namemx.h $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-pathmx.h \
  $(kpathsea_srcdir)/c-stat.h \
  $(kpathsea_srcdir)/c-vararg.h $(kpathsea_srcdir)/cnf.h $(kpathsea_srcdir)/concatn.h \
  $(kpathsea_srcdir)/db.h $(kpathsea_srcdir)/str-list.h $(kpathsea_srcdir)/default.h \
  $(kpathsea_srcdir)/expand.h $(kpathsea_srcdir)/fn.h $(kpathsea_srcdir)/fontmap.h \
  $(kpathsea_srcdir)/hash.h $(kpathsea_srcdir)/getopt.h $(kpathsea_srcdir)/line.h \
  $(kpathsea_srcdir)/magstep.h $(kpathsea_dir)/paths.h $(kpathsea_srcdir)/pathsearch.h \
  $(kpathsea_srcdir)/str-llist.h $(kpathsea_srcdir)/proginit.h $(kpathsea_srcdir)/readable.h \
  $(kpathsea_srcdir)/tex-file.h $(kpathsea_srcdir)/tex-glyph.h $(kpathsea_srcdir)/tex-hush.h \
  $(kpathsea_srcdir)/tex-make.h $(kpathsea_srcdir)/tilde.h $(kpathsea_srcdir)/truncate.h \
  $(kpathsea_srcdir)/variable.h $(kpathsea_srcdir)/xopendir.h $(kpathsea_srcdir)/xstat.h \
  config.h mem.h error.h mfileio.h numbers.h dvi.h pdfdev.h pdfobj.h \
  pdflimits.h pdfdoc.h pdfspecial.h thumbnail.h
pdfobj.o: pdfobj.c \
  system.h \
  $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h \
  $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h \
  $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/absolute.h $(kpathsea_srcdir)/c-ctype.h $(kpathsea_srcdir)/c-dir.h \
  $(kpathsea_srcdir)/c-fopen.h \
  $(kpathsea_srcdir)/c-namemx.h \
  $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-pathmx.h $(kpathsea_srcdir)/c-stat.h \
  $(kpathsea_srcdir)/c-vararg.h \
  $(kpathsea_srcdir)/cnf.h $(kpathsea_srcdir)/concatn.h $(kpathsea_srcdir)/db.h \
  $(kpathsea_srcdir)/str-list.h $(kpathsea_srcdir)/default.h $(kpathsea_srcdir)/expand.h \
  $(kpathsea_srcdir)/fn.h $(kpathsea_srcdir)/fontmap.h $(kpathsea_srcdir)/hash.h \
  $(kpathsea_srcdir)/getopt.h $(kpathsea_srcdir)/line.h $(kpathsea_srcdir)/magstep.h \
  $(kpathsea_dir)/paths.h $(kpathsea_srcdir)/pathsearch.h $(kpathsea_srcdir)/str-llist.h \
  $(kpathsea_srcdir)/proginit.h $(kpathsea_srcdir)/readable.h $(kpathsea_srcdir)/tex-file.h \
  $(kpathsea_srcdir)/tex-glyph.h $(kpathsea_srcdir)/tex-hush.h $(kpathsea_srcdir)/tex-make.h \
  $(kpathsea_srcdir)/tilde.h $(kpathsea_srcdir)/truncate.h $(kpathsea_srcdir)/variable.h \
  $(kpathsea_srcdir)/xopendir.h $(kpathsea_srcdir)/xstat.h config.h mem.h error.h \
  mfileio.h numbers.h pdflimits.h pdfobj.h pdfspecial.h pdfparse.h \
  twiddle.h
pdfparse.o: pdfparse.c \
  system.h $(kpathsea_dir)/c-auto.h \
  $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/absolute.h $(kpathsea_srcdir)/c-ctype.h $(kpathsea_srcdir)/c-dir.h \
  $(kpathsea_srcdir)/c-fopen.h \
  $(kpathsea_srcdir)/c-namemx.h \
  $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-pathmx.h $(kpathsea_srcdir)/c-stat.h \
  $(kpathsea_srcdir)/c-vararg.h \
  $(kpathsea_srcdir)/cnf.h $(kpathsea_srcdir)/concatn.h $(kpathsea_srcdir)/db.h \
  $(kpathsea_srcdir)/str-list.h $(kpathsea_srcdir)/default.h $(kpathsea_srcdir)/expand.h \
  $(kpathsea_srcdir)/fn.h $(kpathsea_srcdir)/fontmap.h $(kpathsea_srcdir)/hash.h \
  $(kpathsea_srcdir)/getopt.h $(kpathsea_srcdir)/line.h $(kpathsea_srcdir)/magstep.h \
  $(kpathsea_dir)/paths.h $(kpathsea_srcdir)/pathsearch.h $(kpathsea_srcdir)/str-llist.h \
  $(kpathsea_srcdir)/proginit.h $(kpathsea_srcdir)/readable.h $(kpathsea_srcdir)/tex-file.h \
  $(kpathsea_srcdir)/tex-glyph.h $(kpathsea_srcdir)/tex-hush.h $(kpathsea_srcdir)/tex-make.h \
  $(kpathsea_srcdir)/tilde.h $(kpathsea_srcdir)/truncate.h $(kpathsea_srcdir)/variable.h \
  $(kpathsea_srcdir)/xopendir.h $(kpathsea_srcdir)/xstat.h mem.h mfileio.h numbers.h \
  dvi.h error.h pdfdev.h pdfobj.h pdfparse.h pdfspecial.h pdfdoc.h
pdfspecial.o: pdfspecial.c \
  system.h $(kpathsea_dir)/c-auto.h \
  $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/absolute.h $(kpathsea_srcdir)/c-ctype.h $(kpathsea_srcdir)/c-dir.h \
  $(kpathsea_srcdir)/c-fopen.h \
  $(kpathsea_srcdir)/c-namemx.h \
  $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-pathmx.h $(kpathsea_srcdir)/c-stat.h \
  $(kpathsea_srcdir)/c-vararg.h \
  $(kpathsea_srcdir)/cnf.h $(kpathsea_srcdir)/concatn.h $(kpathsea_srcdir)/db.h \
  $(kpathsea_srcdir)/str-list.h $(kpathsea_srcdir)/default.h $(kpathsea_srcdir)/expand.h \
  $(kpathsea_srcdir)/fn.h $(kpathsea_srcdir)/fontmap.h $(kpathsea_srcdir)/hash.h \
  $(kpathsea_srcdir)/getopt.h $(kpathsea_srcdir)/line.h $(kpathsea_srcdir)/magstep.h \
  $(kpathsea_dir)/paths.h $(kpathsea_srcdir)/pathsearch.h $(kpathsea_srcdir)/str-llist.h \
  $(kpathsea_srcdir)/proginit.h $(kpathsea_srcdir)/readable.h $(kpathsea_srcdir)/tex-file.h \
  $(kpathsea_srcdir)/tex-glyph.h $(kpathsea_srcdir)/tex-hush.h $(kpathsea_srcdir)/tex-make.h \
  $(kpathsea_srcdir)/tilde.h $(kpathsea_srcdir)/truncate.h $(kpathsea_srcdir)/variable.h \
  $(kpathsea_srcdir)/xopendir.h $(kpathsea_srcdir)/xstat.h mem.h mfileio.h numbers.h \
  dvi.h error.h pdfdev.h pdfobj.h pdflimits.h pdfspecial.h pdfdoc.h \
  pdfparse.h jpeg.h epdf.h mpost.h psimage.h config.h \
  pngimage.h
pkfont.o: pkfont.c \
  system.h \
  $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h \
  $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/absolute.h $(kpathsea_srcdir)/c-ctype.h \
  $(kpathsea_srcdir)/c-dir.h \
  $(kpathsea_srcdir)/c-fopen.h \
  $(kpathsea_srcdir)/c-namemx.h $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-pathmx.h \
  $(kpathsea_srcdir)/c-stat.h \
  $(kpathsea_srcdir)/c-vararg.h $(kpathsea_srcdir)/cnf.h $(kpathsea_srcdir)/concatn.h \
  $(kpathsea_srcdir)/db.h $(kpathsea_srcdir)/str-list.h $(kpathsea_srcdir)/default.h \
  $(kpathsea_srcdir)/expand.h $(kpathsea_srcdir)/fn.h $(kpathsea_srcdir)/fontmap.h \
  $(kpathsea_srcdir)/hash.h $(kpathsea_srcdir)/getopt.h $(kpathsea_srcdir)/line.h \
  $(kpathsea_srcdir)/magstep.h $(kpathsea_dir)/paths.h $(kpathsea_srcdir)/pathsearch.h \
  $(kpathsea_srcdir)/str-llist.h $(kpathsea_srcdir)/proginit.h $(kpathsea_srcdir)/readable.h \
  $(kpathsea_srcdir)/tex-file.h $(kpathsea_srcdir)/tex-glyph.h $(kpathsea_srcdir)/tex-hush.h \
  $(kpathsea_srcdir)/tex-make.h $(kpathsea_srcdir)/tilde.h $(kpathsea_srcdir)/truncate.h \
  $(kpathsea_srcdir)/variable.h $(kpathsea_srcdir)/xopendir.h $(kpathsea_srcdir)/xstat.h mem.h \
  error.h mfileio.h numbers.h pkfont.h pdfobj.h pdflimits.h tfm.h
pngimage.o: pngimage.c \
  $(kpathsea_srcdir)/config.h \
  $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h \
  $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h \
  $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/c-ctype.h system.h \
  $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/absolute.h $(kpathsea_srcdir)/c-dir.h \
  $(kpathsea_srcdir)/c-fopen.h \
  $(kpathsea_srcdir)/c-namemx.h \
  $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-pathmx.h $(kpathsea_srcdir)/c-stat.h \
  $(kpathsea_srcdir)/c-vararg.h \
  $(kpathsea_srcdir)/cnf.h $(kpathsea_srcdir)/concatn.h $(kpathsea_srcdir)/db.h \
  $(kpathsea_srcdir)/str-list.h $(kpathsea_srcdir)/default.h $(kpathsea_srcdir)/expand.h \
  $(kpathsea_srcdir)/fn.h $(kpathsea_srcdir)/fontmap.h $(kpathsea_srcdir)/hash.h \
  $(kpathsea_srcdir)/getopt.h $(kpathsea_srcdir)/line.h $(kpathsea_srcdir)/magstep.h \
  $(kpathsea_dir)/paths.h $(kpathsea_srcdir)/pathsearch.h $(kpathsea_srcdir)/str-llist.h \
  $(kpathsea_srcdir)/proginit.h $(kpathsea_srcdir)/readable.h $(kpathsea_srcdir)/tex-file.h \
  $(kpathsea_srcdir)/tex-glyph.h $(kpathsea_srcdir)/tex-hush.h $(kpathsea_srcdir)/tex-make.h \
  $(kpathsea_srcdir)/tilde.h $(kpathsea_srcdir)/truncate.h $(kpathsea_srcdir)/variable.h \
  $(kpathsea_srcdir)/xopendir.h $(kpathsea_srcdir)/xstat.h config.h mem.h pdfobj.h 
psimage.o: psimage.c \
  system.h \
  $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h \
  $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/absolute.h $(kpathsea_srcdir)/c-ctype.h \
  $(kpathsea_srcdir)/c-dir.h \
  $(kpathsea_srcdir)/c-fopen.h \
  $(kpathsea_srcdir)/c-namemx.h $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-pathmx.h \
  $(kpathsea_srcdir)/c-stat.h \
  $(kpathsea_srcdir)/c-vararg.h $(kpathsea_srcdir)/cnf.h $(kpathsea_srcdir)/concatn.h \
  $(kpathsea_srcdir)/db.h $(kpathsea_srcdir)/str-list.h $(kpathsea_srcdir)/default.h \
  $(kpathsea_srcdir)/expand.h $(kpathsea_srcdir)/fn.h $(kpathsea_srcdir)/fontmap.h \
  $(kpathsea_srcdir)/hash.h $(kpathsea_srcdir)/getopt.h $(kpathsea_srcdir)/line.h \
  $(kpathsea_srcdir)/magstep.h $(kpathsea_dir)/paths.h $(kpathsea_srcdir)/pathsearch.h \
  $(kpathsea_srcdir)/str-llist.h $(kpathsea_srcdir)/proginit.h $(kpathsea_srcdir)/readable.h \
  $(kpathsea_srcdir)/tex-file.h $(kpathsea_srcdir)/tex-glyph.h $(kpathsea_srcdir)/tex-hush.h \
  $(kpathsea_srcdir)/tex-make.h $(kpathsea_srcdir)/tilde.h $(kpathsea_srcdir)/truncate.h \
  $(kpathsea_srcdir)/variable.h $(kpathsea_srcdir)/xopendir.h $(kpathsea_srcdir)/xstat.h \
  config.h mem.h mfileio.h numbers.h pdfobj.h psimage.h pdfspecial.h \
  epdf.h
psspecial.o: psspecial.c \
  system.h \
  $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h \
  $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h \
  $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/absolute.h $(kpathsea_srcdir)/c-ctype.h \
  $(kpathsea_srcdir)/c-dir.h \
  $(kpathsea_srcdir)/c-fopen.h \
  $(kpathsea_srcdir)/c-namemx.h $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-pathmx.h \
  $(kpathsea_srcdir)/c-stat.h \
  $(kpathsea_srcdir)/c-vararg.h $(kpathsea_srcdir)/cnf.h $(kpathsea_srcdir)/concatn.h \
  $(kpathsea_srcdir)/db.h $(kpathsea_srcdir)/str-list.h $(kpathsea_srcdir)/default.h \
  $(kpathsea_srcdir)/expand.h $(kpathsea_srcdir)/fn.h $(kpathsea_srcdir)/fontmap.h \
  $(kpathsea_srcdir)/hash.h $(kpathsea_srcdir)/getopt.h $(kpathsea_srcdir)/line.h \
  $(kpathsea_srcdir)/magstep.h $(kpathsea_dir)/paths.h $(kpathsea_srcdir)/pathsearch.h \
  $(kpathsea_srcdir)/str-llist.h $(kpathsea_srcdir)/proginit.h $(kpathsea_srcdir)/readable.h \
  $(kpathsea_srcdir)/tex-file.h $(kpathsea_srcdir)/tex-glyph.h $(kpathsea_srcdir)/tex-hush.h \
  $(kpathsea_srcdir)/tex-make.h $(kpathsea_srcdir)/tilde.h $(kpathsea_srcdir)/truncate.h \
  $(kpathsea_srcdir)/variable.h $(kpathsea_srcdir)/xopendir.h $(kpathsea_srcdir)/xstat.h mem.h \
  mfileio.h numbers.h psspecial.h pdfparse.h pdfobj.h pdfspecial.h \
  psimage.h mpost.h pdfdoc.h
t1crypt.o: t1crypt.c t1crypt.h
tfm.o: tfm.c \
  system.h \
  $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h \
  $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/absolute.h $(kpathsea_srcdir)/c-ctype.h \
  $(kpathsea_srcdir)/c-dir.h \
  $(kpathsea_srcdir)/c-fopen.h \
  $(kpathsea_srcdir)/c-namemx.h $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-pathmx.h \
  $(kpathsea_srcdir)/c-stat.h \
  $(kpathsea_srcdir)/c-vararg.h $(kpathsea_srcdir)/cnf.h $(kpathsea_srcdir)/concatn.h \
  $(kpathsea_srcdir)/db.h $(kpathsea_srcdir)/str-list.h $(kpathsea_srcdir)/default.h \
  $(kpathsea_srcdir)/expand.h $(kpathsea_srcdir)/fn.h $(kpathsea_srcdir)/fontmap.h \
  $(kpathsea_srcdir)/hash.h $(kpathsea_srcdir)/getopt.h $(kpathsea_srcdir)/line.h \
  $(kpathsea_srcdir)/magstep.h $(kpathsea_dir)/paths.h $(kpathsea_srcdir)/pathsearch.h \
  $(kpathsea_srcdir)/str-llist.h $(kpathsea_srcdir)/proginit.h $(kpathsea_srcdir)/readable.h \
  $(kpathsea_srcdir)/tex-file.h $(kpathsea_srcdir)/tex-glyph.h $(kpathsea_srcdir)/tex-hush.h \
  $(kpathsea_srcdir)/tex-make.h $(kpathsea_srcdir)/tilde.h $(kpathsea_srcdir)/truncate.h \
  $(kpathsea_srcdir)/variable.h $(kpathsea_srcdir)/xopendir.h $(kpathsea_srcdir)/xstat.h mem.h \
  error.h mfileio.h numbers.h pdflimits.h tfm.h config.h
thumbnail.o: thumbnail.c \
  $(kpathsea_srcdir)/config.h \
  $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h \
  $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h \
  $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/c-ctype.h system.h \
  $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/absolute.h $(kpathsea_srcdir)/c-dir.h \
  $(kpathsea_srcdir)/c-fopen.h \
  $(kpathsea_srcdir)/c-namemx.h \
  $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-pathmx.h $(kpathsea_srcdir)/c-stat.h \
  $(kpathsea_srcdir)/c-vararg.h \
  $(kpathsea_srcdir)/cnf.h $(kpathsea_srcdir)/concatn.h $(kpathsea_srcdir)/db.h \
  $(kpathsea_srcdir)/str-list.h $(kpathsea_srcdir)/default.h $(kpathsea_srcdir)/expand.h \
  $(kpathsea_srcdir)/fn.h $(kpathsea_srcdir)/fontmap.h $(kpathsea_srcdir)/hash.h \
  $(kpathsea_srcdir)/getopt.h $(kpathsea_srcdir)/line.h $(kpathsea_srcdir)/magstep.h \
  $(kpathsea_dir)/paths.h $(kpathsea_srcdir)/pathsearch.h $(kpathsea_srcdir)/str-llist.h \
  $(kpathsea_srcdir)/proginit.h $(kpathsea_srcdir)/readable.h $(kpathsea_srcdir)/tex-file.h \
  $(kpathsea_srcdir)/tex-glyph.h $(kpathsea_srcdir)/tex-hush.h $(kpathsea_srcdir)/tex-make.h \
  $(kpathsea_srcdir)/tilde.h $(kpathsea_srcdir)/truncate.h $(kpathsea_srcdir)/variable.h \
  $(kpathsea_srcdir)/xopendir.h $(kpathsea_srcdir)/xstat.h config.h mfileio.h numbers.h \
  mem.h pdfobj.h thumbnail.h \
  pngimage.h
tpic.o: tpic.c \
  system.h $(kpathsea_dir)/c-auto.h \
  $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/absolute.h $(kpathsea_srcdir)/c-ctype.h \
  $(kpathsea_srcdir)/c-dir.h \
  $(kpathsea_srcdir)/c-fopen.h \
  $(kpathsea_srcdir)/c-namemx.h $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-pathmx.h \
  $(kpathsea_srcdir)/c-stat.h \
  $(kpathsea_srcdir)/c-vararg.h $(kpathsea_srcdir)/cnf.h $(kpathsea_srcdir)/concatn.h \
  $(kpathsea_srcdir)/db.h $(kpathsea_srcdir)/str-list.h $(kpathsea_srcdir)/default.h \
  $(kpathsea_srcdir)/expand.h $(kpathsea_srcdir)/fn.h $(kpathsea_srcdir)/fontmap.h \
  $(kpathsea_srcdir)/hash.h $(kpathsea_srcdir)/getopt.h $(kpathsea_srcdir)/line.h \
  $(kpathsea_srcdir)/magstep.h $(kpathsea_dir)/paths.h $(kpathsea_srcdir)/pathsearch.h \
  $(kpathsea_srcdir)/str-llist.h $(kpathsea_srcdir)/proginit.h $(kpathsea_srcdir)/readable.h \
  $(kpathsea_srcdir)/tex-file.h $(kpathsea_srcdir)/tex-glyph.h $(kpathsea_srcdir)/tex-hush.h \
  $(kpathsea_srcdir)/tex-make.h $(kpathsea_srcdir)/tilde.h $(kpathsea_srcdir)/truncate.h \
  $(kpathsea_srcdir)/variable.h $(kpathsea_srcdir)/xopendir.h $(kpathsea_srcdir)/xstat.h mem.h \
  mfileio.h numbers.h tpic.h pdfparse.h pdfobj.h pdfdoc.h pdfdev.h dvi.h \
  error.h
ttf.o: ttf.c config.h ttf.h pdfobj.h \
  system.h \
  $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h \
  $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h \
  $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/absolute.h $(kpathsea_srcdir)/c-ctype.h \
  $(kpathsea_srcdir)/c-dir.h \
  $(kpathsea_srcdir)/c-fopen.h \
  $(kpathsea_srcdir)/c-namemx.h $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-pathmx.h \
  $(kpathsea_srcdir)/c-stat.h \
  $(kpathsea_srcdir)/c-vararg.h $(kpathsea_srcdir)/cnf.h $(kpathsea_srcdir)/concatn.h \
  $(kpathsea_srcdir)/db.h $(kpathsea_srcdir)/str-list.h $(kpathsea_srcdir)/default.h \
  $(kpathsea_srcdir)/expand.h $(kpathsea_srcdir)/fn.h $(kpathsea_srcdir)/fontmap.h \
  $(kpathsea_srcdir)/hash.h $(kpathsea_srcdir)/getopt.h $(kpathsea_srcdir)/line.h \
  $(kpathsea_srcdir)/magstep.h $(kpathsea_dir)/paths.h $(kpathsea_srcdir)/pathsearch.h \
  $(kpathsea_srcdir)/str-llist.h $(kpathsea_srcdir)/proginit.h $(kpathsea_srcdir)/readable.h \
  $(kpathsea_srcdir)/tex-file.h $(kpathsea_srcdir)/tex-glyph.h $(kpathsea_srcdir)/tex-hush.h \
  $(kpathsea_srcdir)/tex-make.h $(kpathsea_srcdir)/tilde.h $(kpathsea_srcdir)/truncate.h \
  $(kpathsea_srcdir)/variable.h $(kpathsea_srcdir)/xopendir.h $(kpathsea_srcdir)/xstat.h mem.h \
  error.h mfileio.h numbers.h tfm.h pdflimits.h twiddle.h encodings.h \
  macglyphs.h
type1.o: type1.c \
  system.h \
  $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h \
  $(kpathsea_srcdir)/c-std.h $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/absolute.h $(kpathsea_srcdir)/c-ctype.h $(kpathsea_srcdir)/c-dir.h \
  $(kpathsea_srcdir)/c-fopen.h \
  $(kpathsea_srcdir)/c-namemx.h \
  $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-pathmx.h $(kpathsea_srcdir)/c-stat.h \
  $(kpathsea_srcdir)/c-vararg.h \
  $(kpathsea_srcdir)/cnf.h $(kpathsea_srcdir)/concatn.h $(kpathsea_srcdir)/db.h \
  $(kpathsea_srcdir)/str-list.h $(kpathsea_srcdir)/default.h $(kpathsea_srcdir)/expand.h \
  $(kpathsea_srcdir)/fn.h $(kpathsea_srcdir)/fontmap.h $(kpathsea_srcdir)/hash.h \
  $(kpathsea_srcdir)/getopt.h $(kpathsea_srcdir)/line.h $(kpathsea_srcdir)/magstep.h \
  $(kpathsea_dir)/paths.h $(kpathsea_srcdir)/pathsearch.h $(kpathsea_srcdir)/str-llist.h \
  $(kpathsea_srcdir)/proginit.h $(kpathsea_srcdir)/readable.h $(kpathsea_srcdir)/tex-file.h \
  $(kpathsea_srcdir)/tex-glyph.h $(kpathsea_srcdir)/tex-hush.h $(kpathsea_srcdir)/tex-make.h \
  $(kpathsea_srcdir)/tilde.h $(kpathsea_srcdir)/truncate.h $(kpathsea_srcdir)/variable.h \
  $(kpathsea_srcdir)/xopendir.h $(kpathsea_srcdir)/xstat.h mem.h error.h mfileio.h \
  numbers.h pdfobj.h type1.h tfm.h pdfparse.h pdflimits.h t1crypt.h \
  twiddle.h encodings.h standardenc.h
vf.o: vf.c \
  system.h $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/kpathsea.h \
  $(kpathsea_srcdir)/config.h $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h \
  $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/absolute.h $(kpathsea_srcdir)/c-ctype.h $(kpathsea_srcdir)/c-dir.h \
  $(kpathsea_srcdir)/c-fopen.h \
  $(kpathsea_srcdir)/c-namemx.h \
  $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-pathmx.h $(kpathsea_srcdir)/c-stat.h \
  $(kpathsea_srcdir)/c-vararg.h \
  $(kpathsea_srcdir)/cnf.h $(kpathsea_srcdir)/concatn.h $(kpathsea_srcdir)/db.h \
  $(kpathsea_srcdir)/str-list.h $(kpathsea_srcdir)/default.h $(kpathsea_srcdir)/expand.h \
  $(kpathsea_srcdir)/fn.h $(kpathsea_srcdir)/fontmap.h $(kpathsea_srcdir)/hash.h \
  $(kpathsea_srcdir)/getopt.h $(kpathsea_srcdir)/line.h $(kpathsea_srcdir)/magstep.h \
  $(kpathsea_dir)/paths.h $(kpathsea_srcdir)/pathsearch.h $(kpathsea_srcdir)/str-llist.h \
  $(kpathsea_srcdir)/proginit.h $(kpathsea_srcdir)/readable.h $(kpathsea_srcdir)/tex-file.h \
  $(kpathsea_srcdir)/tex-glyph.h $(kpathsea_srcdir)/tex-hush.h $(kpathsea_srcdir)/tex-make.h \
  $(kpathsea_srcdir)/tilde.h $(kpathsea_srcdir)/truncate.h $(kpathsea_srcdir)/variable.h \
  $(kpathsea_srcdir)/xopendir.h $(kpathsea_srcdir)/xstat.h mfileio.h numbers.h \
  pdflimits.h mem.h error.h tfm.h pdfdev.h pdfobj.h dvi.h vf.h config.h \
  dvicodes.h
