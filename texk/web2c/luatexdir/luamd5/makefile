# Location of Lua header files
LUA_DIR = /usr/local/include/

# Compilation parameters
CC = gcc
CWARNS = -Wall -Wmissing-prototypes -Wshadow -pedantic -Wpointer-arith -Wcast-align -Waggregate-return
CFLAGS = $(CONFIG) $(CWARNS) -ansi -O2 -I$(LUA_DIR)


# Files in this distribution
FILES = makefile \
	md5.c \
	md5lib.c \
	md5.h \
	md5.lua \
	md5.html \
	md5tests.lua

# Rule to create shared library, to be used with `loadlib' (see also
# file `md5.lua')
md5 : md5.o md5lib.o
	ld -o libmd5.so -shared md5.o md5lib.o

