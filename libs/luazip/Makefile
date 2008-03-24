# $Id: Makefile,v 1.9 2005/06/27 17:43:19 tomas Exp $

T= zip
V= 1.2.1
CONFIG= ./config

include $(CONFIG)

SRCS= src/lua$T.c
OBJS= src/lua$T.o


lib: src/$(LIBNAME)

src/$(LIBNAME): $(OBJS)
	$(CC) $(CFLAGS) $(LIB_OPTION) -o src/$(LIBNAME) $(OBJS) -lzzip

install: src/$(LIBNAME)
	mkdir -p $(LUA_LIBDIR)
	cp src/$(LIBNAME) $(LUA_LIBDIR)
	cd $(LUA_LIBDIR); ln -f -s $(LIBNAME) $T.so

clean:
	rm -f $L src/$(LIBNAME) $(OBJS)
