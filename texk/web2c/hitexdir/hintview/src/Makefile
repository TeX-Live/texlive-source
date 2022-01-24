
CC = gcc
CFLAGS = -g -Og -DDEBUG -Wall
LDFLAGS = -g -Og
#GL_DEFINES = -DGLEW_STATIC -DTW_NO_DIRECT3D -DTW_NO_LIB_PRAGMA -DTW_STATIC -D_CRT_SECURE_NO_WARNINGS

FT_INCLUDES = -I/usr/include/freetype2
GL_INCLUDES = $(FT_INCLUDES)


HFILES =     hint.h htex.h format.h get.h basetypes.h error.h \
	     hrender.h rendernative.h hfonts.h

WFILES =     format.w hint.w htex.ch btex.w

HINT_FILES = get.c hint.c tables.c htex.c hrender.c hfonts.c \
	     $(HFILES) $(WFILES)


OBJ=main.o  renderOGL.o get.o hint.o tables.o htex.o hrender.o hfonts.o

ifeq (Msys,$(shell uname -o))
LIBS= ../external/glfw-3.1.2/msys.build/src/libglfw3.a ../external/libglew.a  -lfreetype \
	       -lm -lz
#	 -ldl -lm -lX11 -lpthread -lXrandr -lXinerama -lXi -lXcursor -lGL -lz
else
LIBS= -lglfw -lGLEW -ldl -lfreetype -lX11 -lXrandr -lXinerama -lXi -lXcursor -lGL -lz -lpthread -lm 
endif

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

SRC= $(OBJ:.o=.c) $(HFILES) main.h stb_image.h Makefile
DIST_SRC = $(SRC:%=src/%)
DIST_MAN = man/hintview.1

hintview.tgz: 
	tar -c -C .. --transform 's,^,hintview/,' -z -f $@ $(DIST_SRC) $(DIST_MAN) 

BINDIR=/usr/local/bin
MANDIR=/usr/local/man

install: hintview
	strip hintview
	install -t $(BINDIR) --mode=755 hintview
	install -t $(MANDIR)/man1 --mode=644 ../man/hintview.1


.PHONY: www
www: hintview hintview.tgz
	cp hintview hintview.tgz ../man/hintview.1 ../../www/linux/

clean:
	rm -f *.o *~ *.dvi *.log *.aux
	rm -f hintview hintview.tgz
	$(MAKE) -C ../../hint viewlinux || true

