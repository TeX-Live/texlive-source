$(LEX_OUTPUT_ROOT).o: $(LEX_OUTPUT_ROOT).c \
  routines.h ../config.h $(kpathsea_srcdir)/config.h $(kpathsea_dir)/c-auto.h \
  $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  ../../web2c/c-auto.h $(kpathsea_srcdir)/c-vararg.h $(kpathsea_srcdir)/c-fopen.h \
  yystype.h y_tab.h
otp2ocp.o: otp2ocp.c $(kpathsea_srcdir)/config.h $(kpathsea_dir)/c-auto.h \
  $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/c-fopen.h \
  $(kpathsea_srcdir)/tex-file.h $(kpathsea_srcdir)/c-vararg.h routines.h \
  ../config.h ../../web2c/c-auto.h
outocp.o: outocp.c $(kpathsea_srcdir)/config.h $(kpathsea_dir)/c-auto.h \
  $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  $(kpathsea_srcdir)/c-fopen.h \
  $(kpathsea_srcdir)/tex-file.h $(kpathsea_srcdir)/c-vararg.h otp.h
routines.o: routines.c $(kpathsea_srcdir)/config.h $(kpathsea_dir)/c-auto.h \
  $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  routines.h ../config.h ../../web2c/c-auto.h $(kpathsea_srcdir)/c-vararg.h \
  $(kpathsea_srcdir)/c-fopen.h \
  otp.h
y_tab.o: y_tab.c otp.h routines.h ../config.h $(kpathsea_srcdir)/config.h \
  $(kpathsea_dir)/c-auto.h $(kpathsea_srcdir)/c-std.h \
  $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
  $(kpathsea_srcdir)/c-memstr.h \
  $(kpathsea_srcdir)/c-errno.h \
  $(kpathsea_srcdir)/c-minmax.h $(kpathsea_srcdir)/c-limits.h \
  $(kpathsea_srcdir)/c-proto.h $(kpathsea_srcdir)/debug.h \
  $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h $(kpathsea_srcdir)/progname.h \
  ../../web2c/c-auto.h $(kpathsea_srcdir)/c-vararg.h $(kpathsea_srcdir)/c-fopen.h \
  yystype.h
