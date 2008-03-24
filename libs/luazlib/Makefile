# $Id: Makefile,v 1.8 2004/07/22 19:10:47 tngd Exp $
# makefile for zlib library for Lua

# dist location
DISTDIR=$(HOME)/dist
TMP=/tmp

# change these to reflect your Lua installation
LUA= $(HOME)/lua
LUAINC= $(LUA)/include
LUALIB= $(LUA)/lib
LUABIN= $(LUA)/bin

ZLIB=../zlib-1.2.1

# no need to change anything below here
CFLAGS= $(INCS) $(DEFS) $(WARN) -O2
WARN= -g -Werror -Wall #-ansi -pedantic -Wall
INCS= -I$(LUAINC) -I$(ZLIB)
LIBS= -L$(ZLIB) -lz

MYNAME= zlib
MYLIB= l$(MYNAME)

OBJS= lzlib.o lgzip.o

T= $(MYLIB).so

VER=0.2
TARFILE = $(DISTDIR)/$(MYLIB)-$(VER).tar.gz
TARFILES = 	Makefile README \
			lzlib.c lgzip.c \
			zlib.lua gzip.lua \
			test_zlib.lua test_gzip.lua

all: $T

#lua: lzlib.o lua.o lauxlib.o
#	gcc -g -o lua lua.o $(OBJS) -I. $(LIBS) -lm -ldl -llualib -llua

test: $T
	$(LUABIN)/lua -lzlib test_zlib.lua
	$(LUABIN)/lua -lgzip test_gzip.lua

$T:	$(OBJS)
	$(CC) -o $@ -shared $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS) $T core core.* a.out

dist: $(TARFILE)

$(TARFILE): $(TARFILES)
	@ln -sf `pwd` $(TMP)/$(MYLIB)-$(VER)
	tar -zcvf $(TARFILE) -C $(TMP) $(addprefix $(MYLIB)-$(VER)/,$(TARFILES))
	@rm -f $(TMP)/$(MYLIB)-$(VER)
	@lsum $(TARFILE) $(DISTDIR)/md5sums.txt
