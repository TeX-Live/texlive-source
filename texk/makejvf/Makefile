CC = gcc
CCLD =$(CC)
PROGRAM = makejvf

# インストール先を DISTDIR に指定する
DISTDIR = /usr/local/bin

CFLAGS = -O
PROGLINK = $(CC) $(OBJS) -o $(PROGRAM)

OBJS = main.o tfmread.o tool.o write.o

link_command = $(CCLD) -o $(PROGRAM) $(LDFLAGS) 

$(PROGRAM): $(OBJS) $(kpathsea)
	$(PROGLINK)

main.o:main.c
tfmread.o:tfmread.c
tool.o:tool.c
write.o:write.c

install:	$(PROGRAM)
	install -C -m 755 $? $(DISTDIR)

clean:
	rm -f *.o makejvf

distclean:
	rm -f *.o *.core *.tfm *.vf *.tex *.dvi *.ps $(PROGRAM)
