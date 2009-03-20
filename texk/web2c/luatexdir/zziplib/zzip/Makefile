
top_srcdir = ../../../../libs/zziplib
srcdir = $(top_srcdir)/zzip

CC = gcc
AR = ar cru
RANLIB = ranlib

OBJECTS =dir.o err.o fetch.o file.o info.o memdisk.o mmapped.o \
         plugin.o stat.o write.o zip.o

_CFLAGS = -DHAVE_CONFIG_H -I.. -I../../../../libs/zziplib -I../../../../libs/zlib

XCFLAGS = $(_CFLAGS)

all: $(OBJECTS)
	$(AR) libzzip.a $(OBJECTS)
	$(RANLIB) libzzip.a

%.o: $(srcdir)/%.c 
	$(CC) $(CFLAGS) $(XCFLAGS) -c $< -o $@

