arith.o: arith.c types.h $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h \
  $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/c-std.h \
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
  c-auto.h objects.h spaces.h arith.h
basename.o: basename.c \
  basics.h \
  $(kpathsea_srcdir)/config.h $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-ctype.h \
  $(kpathsea_srcdir)/c-fopen.h \
  c-auto.h filenames.h
basics.o: basics.c 
bstring.o: bstring.c c-auto.h
curves.o: curves.c types.h $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h \
  $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/c-std.h \
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
  c-auto.h objects.h spaces.h paths.h regions.h curves.h lines.h arith.h
encoding.o: encoding.c \
  basics.h $(kpathsea_srcdir)/config.h $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-ctype.h $(kpathsea_srcdir)/c-fopen.h \
  c-auto.h 
filenames.o: filenames.c \
  $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h \
  $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/c-std.h \
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
  $(kpathsea_srcdir)/xopendir.h $(kpathsea_srcdir)/xstat.h c-auto.h basics.h
flisearch.o: flisearch.c \
  basics.h $(kpathsea_srcdir)/config.h \
  $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h \
  $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-ctype.h \
  $(kpathsea_srcdir)/c-fopen.h \
  c-auto.h filenames.h \
  psearch.h
fontfcn.o: fontfcn.c \
  t1imager.h types.h $(kpathsea_srcdir)/kpathsea.h \
  $(kpathsea_srcdir)/config.h $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
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
  $(kpathsea_srcdir)/variable.h $(kpathsea_srcdir)/xopendir.h $(kpathsea_srcdir)/xstat.h \
  c-auto.h fontmisc.h util.h fontfcn.h
hints.o: hints.c types.h $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h \
  $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/c-std.h \
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
  c-auto.h objects.h spaces.h paths.h regions.h hints.h
lines.o: lines.c types.h $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h \
  $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/c-std.h \
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
  c-auto.h objects.h spaces.h regions.h lines.h
mag.o: mag.c \
  basics.h $(kpathsea_srcdir)/config.h $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-ctype.h $(kpathsea_srcdir)/c-fopen.h \
  c-auto.h 
objects.o: objects.c types.h $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h \
  $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/c-std.h \
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
  c-auto.h objects.h spaces.h paths.h regions.h fonts.h pictures.h \
  strokes.h cluts.h
paths.o: paths.c types.h $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h \
  $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/c-std.h \
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
  c-auto.h objects.h spaces.h paths.h regions.h fonts.h pictures.h \
  strokes.h trig.h
pfb2pfa.o: pfb2pfa.c \
  basics.h $(kpathsea_srcdir)/config.h \
  $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h \
  $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-ctype.h \
  $(kpathsea_srcdir)/c-fopen.h \
  c-auto.h filenames.h
pk2bm.o: pk2bm.c \
  pkin.h 
pkin.o: pkin.c \
  basics.h \
  $(kpathsea_srcdir)/config.h $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h \
  $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-ctype.h \
  $(kpathsea_srcdir)/c-fopen.h \
  c-auto.h pkin.h
pkout.o: pkout.c \
  basics.h \
  $(kpathsea_srcdir)/config.h $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h \
  $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-ctype.h \
  $(kpathsea_srcdir)/c-fopen.h \
  c-auto.h pkout.h
pktest.o: pktest.c \
  basics.h \
  $(kpathsea_srcdir)/config.h $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-ctype.h \
  $(kpathsea_srcdir)/c-fopen.h \
  c-auto.h pkout.h
ps2pk.o: ps2pk.c \
  basics.h $(kpathsea_srcdir)/config.h $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-ctype.h $(kpathsea_srcdir)/c-fopen.h \
  c-auto.h \
  filenames.h psearch.h \
  pkout.h ffilest.h types.h $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/absolute.h \
  $(kpathsea_srcdir)/c-dir.h \
  $(kpathsea_srcdir)/c-namemx.h $(kpathsea_srcdir)/c-pathmx.h $(kpathsea_srcdir)/c-stat.h \
  $(kpathsea_srcdir)/c-vararg.h $(kpathsea_srcdir)/cnf.h $(kpathsea_srcdir)/concatn.h \
  $(kpathsea_srcdir)/db.h $(kpathsea_srcdir)/str-list.h $(kpathsea_srcdir)/default.h \
  $(kpathsea_srcdir)/expand.h $(kpathsea_srcdir)/fn.h $(kpathsea_srcdir)/fontmap.h \
  $(kpathsea_srcdir)/hash.h $(kpathsea_srcdir)/getopt.h $(kpathsea_srcdir)/line.h \
  $(kpathsea_srcdir)/magstep.h $(kpathsea_dir)/paths.h $(kpathsea_srcdir)/pathsearch.h \
  $(kpathsea_srcdir)/str-llist.h $(kpathsea_srcdir)/proginit.h $(kpathsea_srcdir)/readable.h \
  $(kpathsea_srcdir)/tex-file.h $(kpathsea_srcdir)/tex-glyph.h $(kpathsea_srcdir)/tex-hush.h \
  $(kpathsea_srcdir)/tex-make.h $(kpathsea_srcdir)/tilde.h $(kpathsea_srcdir)/truncate.h \
  $(kpathsea_srcdir)/variable.h $(kpathsea_srcdir)/xopendir.h $(kpathsea_srcdir)/xstat.h \
  Xstuff.h fontmisc.h fontstruct.h font.h fsmasks.h fontfile.h fontxlfd.h
psearch.o: psearch.c \
  basics.h \
  $(kpathsea_srcdir)/config.h $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-ctype.h $(kpathsea_srcdir)/c-fopen.h \
  c-auto.h strexpr.h \
  filenames.h texfiles.h
regions.o: regions.c types.h $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h \
  $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/c-std.h \
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
  c-auto.h objects.h spaces.h regions.h paths.h curves.h lines.h \
  pictures.h fonts.h hints.h strokes.h
scanfont.o: scanfont.c \
  types.h \
  $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h $(kpathsea_dir)/c-auto.h \
  $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
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
  $(kpathsea_srcdir)/variable.h $(kpathsea_srcdir)/xopendir.h $(kpathsea_srcdir)/xstat.h \
  c-auto.h t1stdio.h util.h token.h fontfcn.h blues.h
sexpr.o: sexpr.c \
  strexpr.h \
  basics.h $(kpathsea_srcdir)/config.h $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-ctype.h \
  $(kpathsea_srcdir)/c-fopen.h \
  c-auto.h
spaces.o: spaces.c types.h $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h \
  $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/c-std.h \
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
  c-auto.h objects.h spaces.h paths.h pictures.h fonts.h arith.h trig.h
strexpr.o: strexpr.c \
  basics.h $(kpathsea_srcdir)/config.h $(kpathsea_dir)/c-auto.h \
  $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/c-pathch.h $(kpathsea_srcdir)/c-ctype.h $(kpathsea_srcdir)/c-fopen.h \
  c-auto.h 
t1funcs.o: t1funcs.c types.h $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h \
  $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/c-std.h \
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
  c-auto.h ffilest.h Xstuff.h fontmisc.h fontstruct.h font.h fsmasks.h \
  fontfile.h fontxlfd.h t1intf.h objects.h spaces.h regions.h t1stdio.h \
  util.h fontfcn.h
t1info.o: t1info.c types.h $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h \
  $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/c-std.h \
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
  c-auto.h ffilest.h Xstuff.h fontmisc.h fontstruct.h font.h fsmasks.h \
  fontfile.h fontxlfd.h t1intf.h
t1io.o: t1io.c c-auto.h \
  t1stdio.h \
  types.h $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h \
  $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/absolute.h $(kpathsea_srcdir)/c-ctype.h \
  $(kpathsea_srcdir)/c-dir.h \
  $(kpathsea_srcdir)/c-fopen.h $(kpathsea_srcdir)/c-namemx.h $(kpathsea_srcdir)/c-pathch.h \
  $(kpathsea_srcdir)/c-pathmx.h $(kpathsea_srcdir)/c-stat.h \
  $(kpathsea_srcdir)/c-vararg.h $(kpathsea_srcdir)/cnf.h \
  $(kpathsea_srcdir)/concatn.h $(kpathsea_srcdir)/db.h $(kpathsea_srcdir)/str-list.h \
  $(kpathsea_srcdir)/default.h $(kpathsea_srcdir)/expand.h $(kpathsea_srcdir)/fn.h \
  $(kpathsea_srcdir)/fontmap.h $(kpathsea_srcdir)/hash.h $(kpathsea_srcdir)/getopt.h \
  $(kpathsea_srcdir)/line.h $(kpathsea_srcdir)/magstep.h $(kpathsea_dir)/paths.h \
  $(kpathsea_srcdir)/pathsearch.h $(kpathsea_srcdir)/str-llist.h $(kpathsea_srcdir)/proginit.h \
  $(kpathsea_srcdir)/readable.h $(kpathsea_srcdir)/tex-file.h $(kpathsea_srcdir)/tex-glyph.h \
  $(kpathsea_srcdir)/tex-hush.h $(kpathsea_srcdir)/tex-make.h $(kpathsea_srcdir)/tilde.h \
  $(kpathsea_srcdir)/truncate.h $(kpathsea_srcdir)/variable.h $(kpathsea_srcdir)/xopendir.h \
  $(kpathsea_srcdir)/xstat.h t1hdigit.h
t1snap.o: t1snap.c objects.h types.h $(kpathsea_srcdir)/kpathsea.h \
  $(kpathsea_srcdir)/config.h $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/c-std.h \
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
  c-auto.h spaces.h paths.h
t1stub.o: t1stub.c objects.h types.h $(kpathsea_srcdir)/kpathsea.h \
  $(kpathsea_srcdir)/config.h $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/c-std.h \
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
  c-auto.h
t1test.o: t1test.c \
  ffilest.h types.h \
  $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h $(kpathsea_dir)/c-auto.h \
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
  c-auto.h Xstuff.h fontmisc.h fontstruct.h font.h fsmasks.h fontfile.h \
  fontxlfd.h
texfiles.o: texfiles.c \
  texfiles.h
token.o: token.c types.h $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h \
  $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/c-std.h \
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
  c-auto.h t1stdio.h util.h digit.h token.h tokst.h hdigit.h
type1.o: type1.c types.h $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h \
  $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/c-std.h \
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
  c-auto.h objects.h spaces.h paths.h fonts.h pictures.h util.h blues.h
util.o: util.c types.h $(kpathsea_srcdir)/kpathsea.h $(kpathsea_srcdir)/config.h \
  $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/c-std.h \
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
  c-auto.h util.h fontmisc.h
