# $Id$
top_srcdir = ../../../../libs/luafontforge/fontforge
srcdir = $(top_srcdir)/Unicode

CC = gcc

OBJECTS = alphabet.o char.o memory.o cjk.o ustring.o gwwiconv.o

_CFLAGS = -I$(top_srcdir)/inc -Wall -DLUA_FF_LIB=1 $(XCFLAGS)
CFLAGS = -g -O2

all: $(OBJECTS)

%.o: $(srcdir)/%.c 
	$(CC) $(_CFLAGS) $(CFLAGS) -c $< -o $@
