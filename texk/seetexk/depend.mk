bcopy.o: bcopy.c
bzero.o: bzero.c
conv.o: conv.c types.h $(kpathsea_srcdir)/config.h c-auto.h \
 $(kpathsea_srcdir)/c-std.h \
 $(kpathsea_srcdir)/c-unistd.h \
 $(kpathsea_srcdir)/systypes.h \
 $(kpathsea_srcdir)/c-memstr.h $(kpathsea_srcdir)/c-errno.h \
 $(kpathsea_srcdir)/c-minmax.h \
 $(kpathsea_srcdir)/c-limits.h \
 $(kpathsea_srcdir)/c-proto.h \
 $(kpathsea_srcdir)/debug.h $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h \
 $(kpathsea_srcdir)/progname.h conv.h
dvi_draw.o: dvi_draw.c 
dvibook.o: dvibook.c types.h $(kpathsea_srcdir)/config.h c-auto.h \
 $(kpathsea_srcdir)/c-std.h \
 $(kpathsea_srcdir)/c-unistd.h \
 $(kpathsea_srcdir)/systypes.h \
 $(kpathsea_srcdir)/c-memstr.h $(kpathsea_srcdir)/c-errno.h \
 $(kpathsea_srcdir)/c-minmax.h \
 $(kpathsea_srcdir)/c-limits.h \
 $(kpathsea_srcdir)/c-proto.h \
 $(kpathsea_srcdir)/debug.h $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h \
 $(kpathsea_srcdir)/progname.h dviclass.h dvicodes.h error.h fio.h gripes.h \
 search.h seek.h
dviclass.o: dviclass.c dviclass.h
dviconcat.o: dviconcat.c types.h $(kpathsea_srcdir)/config.h c-auto.h \
 $(kpathsea_srcdir)/c-std.h \
 $(kpathsea_srcdir)/c-unistd.h \
 $(kpathsea_srcdir)/systypes.h \
 $(kpathsea_srcdir)/c-memstr.h $(kpathsea_srcdir)/c-errno.h \
 $(kpathsea_srcdir)/c-minmax.h \
 $(kpathsea_srcdir)/c-limits.h \
 $(kpathsea_srcdir)/c-proto.h \
 $(kpathsea_srcdir)/debug.h $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h \
 $(kpathsea_srcdir)/progname.h dviclass.h dvicodes.h error.h fio.h gripes.h \
 search.h 
dviselect.o: dviselect.c types.h $(kpathsea_srcdir)/config.h c-auto.h \
 $(kpathsea_srcdir)/c-std.h \
 $(kpathsea_srcdir)/c-unistd.h \
 $(kpathsea_srcdir)/systypes.h \
 $(kpathsea_srcdir)/c-memstr.h $(kpathsea_srcdir)/c-errno.h \
 $(kpathsea_srcdir)/c-minmax.h \
 $(kpathsea_srcdir)/c-limits.h \
 $(kpathsea_srcdir)/c-proto.h \
 $(kpathsea_srcdir)/debug.h $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h \
 $(kpathsea_srcdir)/progname.h dviclass.h dvicodes.h error.h fio.h gripes.h \
 search.h 
dvistate.o: dvistate.c \
 types.h $(kpathsea_srcdir)/config.h c-auto.h \
 $(kpathsea_srcdir)/c-std.h \
 $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
 $(kpathsea_srcdir)/c-memstr.h \
 $(kpathsea_srcdir)/c-errno.h \
 $(kpathsea_srcdir)/c-minmax.h \
 $(kpathsea_srcdir)/c-limits.h \
 $(kpathsea_srcdir)/c-proto.h \
 $(kpathsea_srcdir)/debug.h $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h \
 $(kpathsea_srcdir)/progname.h conv.h dvicodes.h error.h fio.h font.h \
 gripes.h postamble.h search.h seek.h dvistate.h
dvistuff.o: dvistuff.c \
 dvistuff.h types.h $(kpathsea_srcdir)/config.h \
 c-auto.h $(kpathsea_srcdir)/c-std.h \
 $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
 $(kpathsea_srcdir)/c-memstr.h $(kpathsea_srcdir)/c-errno.h \
 $(kpathsea_srcdir)/c-minmax.h \
 $(kpathsea_srcdir)/c-limits.h \
 $(kpathsea_srcdir)/c-proto.h \
 $(kpathsea_srcdir)/debug.h $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h \
 $(kpathsea_srcdir)/progname.h font.h conv.h dviclass.h dvicodes.h \
 postamble.h search.h fio.h
dvitodvi.o: dvitodvi.c types.h $(kpathsea_srcdir)/config.h c-auto.h \
 $(kpathsea_srcdir)/c-std.h \
 $(kpathsea_srcdir)/c-unistd.h \
 $(kpathsea_srcdir)/systypes.h \
 $(kpathsea_srcdir)/c-memstr.h $(kpathsea_srcdir)/c-errno.h \
 $(kpathsea_srcdir)/c-minmax.h \
 $(kpathsea_srcdir)/c-limits.h \
 $(kpathsea_srcdir)/c-proto.h \
 $(kpathsea_srcdir)/debug.h $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h \
 $(kpathsea_srcdir)/progname.h dviclass.h dvicodes.h error.h fio.h gripes.h \
 search.h seek.h
error.o: error.c \
 types.h $(kpathsea_srcdir)/config.h c-auto.h \
 $(kpathsea_srcdir)/c-std.h \
 $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
 $(kpathsea_srcdir)/c-memstr.h \
 $(kpathsea_srcdir)/c-errno.h \
 $(kpathsea_srcdir)/c-minmax.h \
 $(kpathsea_srcdir)/c-limits.h \
 $(kpathsea_srcdir)/c-proto.h \
 $(kpathsea_srcdir)/debug.h $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h \
 $(kpathsea_srcdir)/progname.h error.h
findpost.o: findpost.c \
 types.h $(kpathsea_srcdir)/config.h c-auto.h \
 $(kpathsea_srcdir)/c-std.h \
 $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
 $(kpathsea_srcdir)/c-memstr.h \
 $(kpathsea_srcdir)/c-errno.h \
 $(kpathsea_srcdir)/c-minmax.h \
 $(kpathsea_srcdir)/c-limits.h \
 $(kpathsea_srcdir)/c-proto.h \
 $(kpathsea_srcdir)/debug.h $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h \
 $(kpathsea_srcdir)/progname.h dvicodes.h fio.h num.h
fio.o: fio.c \
 types.h $(kpathsea_srcdir)/config.h c-auto.h \
 $(kpathsea_srcdir)/c-std.h \
 $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
 $(kpathsea_srcdir)/c-memstr.h \
 $(kpathsea_srcdir)/c-errno.h \
 $(kpathsea_srcdir)/c-minmax.h \
 $(kpathsea_srcdir)/c-limits.h \
 $(kpathsea_srcdir)/c-proto.h \
 $(kpathsea_srcdir)/debug.h $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h \
 $(kpathsea_srcdir)/progname.h error.h fio.h
font.o: font.c \
 types.h $(kpathsea_srcdir)/config.h c-auto.h $(kpathsea_srcdir)/c-std.h \
 $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
 $(kpathsea_srcdir)/c-memstr.h $(kpathsea_srcdir)/c-errno.h \
 $(kpathsea_srcdir)/c-minmax.h $(kpathsea_srcdir)/c-limits.h \
 $(kpathsea_srcdir)/c-proto.h \
 $(kpathsea_srcdir)/debug.h $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h \
 $(kpathsea_srcdir)/progname.h conv.h error.h font.h
font_subr.o: font_subr.c types.h $(kpathsea_srcdir)/config.h c-auto.h \
 $(kpathsea_srcdir)/c-std.h \
 $(kpathsea_srcdir)/c-unistd.h \
 $(kpathsea_srcdir)/systypes.h \
 $(kpathsea_srcdir)/c-memstr.h $(kpathsea_srcdir)/c-errno.h \
 $(kpathsea_srcdir)/c-minmax.h \
 $(kpathsea_srcdir)/c-limits.h \
 $(kpathsea_srcdir)/c-proto.h \
 $(kpathsea_srcdir)/debug.h $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h \
 $(kpathsea_srcdir)/progname.h error.h font.h
getopt.o: getopt.c 
gfclass.o: gfclass.c gfclass.h
gffont.o: gffont.c \
 types.h \
 $(kpathsea_srcdir)/config.h c-auto.h $(kpathsea_srcdir)/c-std.h \
 $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
 $(kpathsea_srcdir)/c-memstr.h \
 $(kpathsea_srcdir)/c-errno.h \
 $(kpathsea_srcdir)/c-minmax.h \
 $(kpathsea_srcdir)/c-limits.h \
 $(kpathsea_srcdir)/c-proto.h \
 $(kpathsea_srcdir)/debug.h $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h \
 $(kpathsea_srcdir)/progname.h error.h font.h gfcodes.h gfclass.h num.h
gripes0.o: gripes0.c \
 types.h $(kpathsea_srcdir)/config.h c-auto.h \
 $(kpathsea_srcdir)/c-std.h \
 $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
 $(kpathsea_srcdir)/c-memstr.h \
 $(kpathsea_srcdir)/c-errno.h \
 $(kpathsea_srcdir)/c-minmax.h \
 $(kpathsea_srcdir)/c-limits.h \
 $(kpathsea_srcdir)/c-proto.h \
 $(kpathsea_srcdir)/debug.h $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h \
 $(kpathsea_srcdir)/progname.h error.h gripes.h
gripes1.o: gripes1.c \
 types.h $(kpathsea_srcdir)/config.h c-auto.h \
 $(kpathsea_srcdir)/c-std.h \
 $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
 $(kpathsea_srcdir)/c-memstr.h \
 $(kpathsea_srcdir)/c-errno.h \
 $(kpathsea_srcdir)/c-minmax.h \
 $(kpathsea_srcdir)/c-limits.h \
 $(kpathsea_srcdir)/c-proto.h \
 $(kpathsea_srcdir)/debug.h $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h \
 $(kpathsea_srcdir)/progname.h error.h font.h gripes.h
magfactor.o: magfactor.c
pkfont.o: pkfont.c \
 types.h \
 $(kpathsea_srcdir)/config.h c-auto.h $(kpathsea_srcdir)/c-std.h \
 $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
 $(kpathsea_srcdir)/c-memstr.h \
 $(kpathsea_srcdir)/c-errno.h \
 $(kpathsea_srcdir)/c-minmax.h \
 $(kpathsea_srcdir)/c-limits.h \
 $(kpathsea_srcdir)/c-proto.h \
 $(kpathsea_srcdir)/debug.h $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h \
 $(kpathsea_srcdir)/progname.h error.h font.h num.h
pxlfont.o: pxlfont.c \
 types.h $(kpathsea_srcdir)/config.h c-auto.h \
 $(kpathsea_srcdir)/c-std.h \
 $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
 $(kpathsea_srcdir)/c-memstr.h \
 $(kpathsea_srcdir)/c-errno.h $(kpathsea_srcdir)/c-minmax.h \
 $(kpathsea_srcdir)/c-limits.h \
 $(kpathsea_srcdir)/c-proto.h \
 $(kpathsea_srcdir)/debug.h $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h \
 $(kpathsea_srcdir)/progname.h error.h font.h fio.h
rotate.o: rotate.c types.h $(kpathsea_srcdir)/config.h c-auto.h \
 $(kpathsea_srcdir)/c-std.h \
 $(kpathsea_srcdir)/c-unistd.h \
 $(kpathsea_srcdir)/systypes.h \
 $(kpathsea_srcdir)/c-memstr.h $(kpathsea_srcdir)/c-errno.h \
 $(kpathsea_srcdir)/c-minmax.h \
 $(kpathsea_srcdir)/c-limits.h \
 $(kpathsea_srcdir)/c-proto.h \
 $(kpathsea_srcdir)/debug.h $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h \
 $(kpathsea_srcdir)/progname.h error.h font.h
rstfont.o: rstfont.c \
 types.h $(kpathsea_srcdir)/config.h c-auto.h \
 $(kpathsea_srcdir)/c-std.h \
 $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
 $(kpathsea_srcdir)/c-memstr.h \
 $(kpathsea_srcdir)/c-errno.h $(kpathsea_srcdir)/c-minmax.h \
 $(kpathsea_srcdir)/c-limits.h \
 $(kpathsea_srcdir)/c-proto.h \
 $(kpathsea_srcdir)/debug.h $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h \
 $(kpathsea_srcdir)/progname.h error.h font.h fio.h
scaletfm.o: scaletfm.c types.h $(kpathsea_srcdir)/config.h c-auto.h \
 $(kpathsea_srcdir)/c-std.h \
 $(kpathsea_srcdir)/c-unistd.h \
 $(kpathsea_srcdir)/systypes.h \
 $(kpathsea_srcdir)/c-memstr.h $(kpathsea_srcdir)/c-errno.h \
 $(kpathsea_srcdir)/c-minmax.h \
 $(kpathsea_srcdir)/c-limits.h \
 $(kpathsea_srcdir)/c-proto.h \
 $(kpathsea_srcdir)/debug.h $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h \
 $(kpathsea_srcdir)/progname.h error.h font.h
scanpost.o: scanpost.c \
 types.h $(kpathsea_srcdir)/config.h c-auto.h \
 $(kpathsea_srcdir)/c-std.h \
 $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
 $(kpathsea_srcdir)/c-memstr.h \
 $(kpathsea_srcdir)/c-errno.h \
 $(kpathsea_srcdir)/c-minmax.h \
 $(kpathsea_srcdir)/c-limits.h \
 $(kpathsea_srcdir)/c-proto.h \
 $(kpathsea_srcdir)/debug.h $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h \
 $(kpathsea_srcdir)/progname.h dvicodes.h fio.h gripes.h postamble.h
search.o: search.c types.h $(kpathsea_srcdir)/config.h c-auto.h \
 $(kpathsea_srcdir)/c-std.h \
 $(kpathsea_srcdir)/c-unistd.h \
 $(kpathsea_srcdir)/systypes.h \
 $(kpathsea_srcdir)/c-memstr.h $(kpathsea_srcdir)/c-errno.h \
 $(kpathsea_srcdir)/c-minmax.h \
 $(kpathsea_srcdir)/c-limits.h \
 $(kpathsea_srcdir)/c-proto.h \
 $(kpathsea_srcdir)/debug.h $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h \
 $(kpathsea_srcdir)/progname.h search.h
seek.o: seek.c \
 types.h $(kpathsea_srcdir)/config.h c-auto.h \
 $(kpathsea_srcdir)/c-std.h \
 $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
 $(kpathsea_srcdir)/c-memstr.h \
 $(kpathsea_srcdir)/c-errno.h \
 $(kpathsea_srcdir)/c-minmax.h \
 $(kpathsea_srcdir)/c-limits.h \
 $(kpathsea_srcdir)/c-proto.h \
 $(kpathsea_srcdir)/debug.h $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h \
 $(kpathsea_srcdir)/progname.h seek.h 
skip.o: skip.c \
 types.h $(kpathsea_srcdir)/config.h c-auto.h \
 $(kpathsea_srcdir)/c-std.h \
 $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
 $(kpathsea_srcdir)/c-memstr.h \
 $(kpathsea_srcdir)/c-errno.h \
 $(kpathsea_srcdir)/c-minmax.h \
 $(kpathsea_srcdir)/c-limits.h \
 $(kpathsea_srcdir)/c-proto.h \
 $(kpathsea_srcdir)/debug.h $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h \
 $(kpathsea_srcdir)/progname.h fio.h
split.o: split.c 
strsave.o: strsave.c types.h $(kpathsea_srcdir)/config.h c-auto.h \
 $(kpathsea_srcdir)/c-std.h \
 $(kpathsea_srcdir)/c-unistd.h \
 $(kpathsea_srcdir)/systypes.h \
 $(kpathsea_srcdir)/c-memstr.h $(kpathsea_srcdir)/c-errno.h \
 $(kpathsea_srcdir)/c-minmax.h \
 $(kpathsea_srcdir)/c-limits.h \
 $(kpathsea_srcdir)/c-proto.h \
 $(kpathsea_srcdir)/debug.h $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h \
 $(kpathsea_srcdir)/progname.h error.h
strtol.o: strtol.c 
tempfile.o: tempfile.c 
tfm.o: tfm.c \
 types.h $(kpathsea_srcdir)/config.h c-auto.h \
 $(kpathsea_srcdir)/c-std.h \
 $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
 $(kpathsea_srcdir)/c-memstr.h \
 $(kpathsea_srcdir)/c-errno.h \
 $(kpathsea_srcdir)/c-minmax.h \
 $(kpathsea_srcdir)/c-limits.h \
 $(kpathsea_srcdir)/c-proto.h \
 $(kpathsea_srcdir)/debug.h $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h \
 $(kpathsea_srcdir)/progname.h fio.h tfm.h
tfmfont.o: tfmfont.c \
 types.h \
 $(kpathsea_srcdir)/config.h c-auto.h $(kpathsea_srcdir)/c-std.h \
 $(kpathsea_srcdir)/c-unistd.h $(kpathsea_srcdir)/systypes.h \
 $(kpathsea_srcdir)/c-memstr.h \
 $(kpathsea_srcdir)/c-errno.h \
 $(kpathsea_srcdir)/c-minmax.h \
 $(kpathsea_srcdir)/c-limits.h \
 $(kpathsea_srcdir)/c-proto.h \
 $(kpathsea_srcdir)/debug.h $(kpathsea_srcdir)/types.h $(kpathsea_srcdir)/lib.h \
 $(kpathsea_srcdir)/progname.h conv.h font.h tfm.h
