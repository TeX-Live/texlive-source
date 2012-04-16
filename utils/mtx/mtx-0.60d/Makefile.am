SUBDIRS = libp2c
DIST_SUBDIRS = libp2c
bin_PROGRAMS = prepmx
INCLUDES = -I$(srcdir)/libp2c
prepmx_SOURCES =  analyze.h cfuncs.h config.h control.h files.h globals.h lyrics.h\
  mtx.h mtxline.h multfile.h notes.h preamble.h status.h strings.h uptext.h utility.h\
  globals.c mtx.c preamble.c lyrics.c analyze.c mtxline.c\
  status.c uptext.c files.c notes.c prepmx.c cfuncs.c multfile.c utility.c control.c 
prepmx_LDADD = libp2c/libp2c.a
EXTRA_DIST =  globals.pas mtx.pas preamble.pas lyrics.pas analyze.pas mtxline.pas\
  status.pas uptext.pas prepmx.pas files.pas notes.pas multfile.pas utility.pas control.pas\
  Makefile.orig Bugs  README.060d 

