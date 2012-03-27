SUBDIRS = libf2c
DIST_SUBDIRS = libf2c
bin_PROGRAMS = pmxab scor2prt
pmxab_SOURCES =  libf2c/f2c.h pmx2617.c 
scor2prt_SOURCES =  libf2c/f2c.h scor2prt.c 
pmxab_LDADD = libf2c/libf2c.a
scor2prt_LDADD = libf2c/libf2c.a
EXTRA_DIST = pmx2617.f scor2prt.f
