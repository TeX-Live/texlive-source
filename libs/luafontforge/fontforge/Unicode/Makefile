
top_srcdir = ../../../../src/libs/luafontforge/fontforge
srcdir = $(top_srcdir)/Unicode

CC = gcc

OBJECTS = alphabet.o char.o memory.o cjk.o ustring.o gwwiconv.o

CFLAGS = -I$(top_srcdir)/inc -Wall -g -O2 -DLUA_FF_LIB=1 $(XCFLAGS)

all: $(OBJECTS)

%.o: $(srcdir)/%.c 
	$(CC) $(CFLAGS) -c $< -o $@

