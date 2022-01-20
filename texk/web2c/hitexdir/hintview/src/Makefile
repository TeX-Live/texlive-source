
CC = gcc
CFLAGS = -g -Og -DDEBUG -Wall
LDFLAGS = -g -Og

FT_INCLUDES = -I/usr/include/freetype2


OBJ=main.o  renderOGL.o get.o hint.o tables.o htex.o hrender.o hfonts.o

LIBS= -lglfw -lGLEW -ldl -lfreetype -lX11 -lXrandr -lXinerama -lXi -lXcursor -lGL -lz -lpthread -lm 

all: hintview

.SUFFIXES:

hint.o: hint.c
	$(CC) $(CFLAGS) -Wno-unused-but-set-variable -o $@ -c $<

htex.o: htex.c
	$(CC) $(CFLAGS) -Wno-parentheses -Wno-unused-but-set-variable -Wno-maybe-uninitialized -o $@ -c $<

renderOGL.o: renderOGL.c
	$(CC) $(FT_INCLUDES) $(CFLAGS) -o $@ -c $<

hfonts.o: hfonts.c
	$(CC) $(CFLAGS) $(FT_INCLUDES) -o $@ -c $<

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

hintview: $(OBJ)
	$(CC) $(LDFLAGS)  $(OBJ) $(LIBS) -o $@ 

BINDIR=/usr/local/bin
MANDIR=/usr/local/man

install: hintview
	strip hintview
	install -t $(BINDIR) --mode=755 hintview
	install -t $(MANDIR)/man1 --mode=644 ../man/hintview.1

clean:
	rm -f *.o *~ *.dvi *.log *.aux
	rm -f hintview

