# src/Makefile

OBCDIR = /usr/local/obc-3.2

OBC = $(OBCDIR)/bin/obc
OBLINK = $(OBCDIR)/lib/obc/oblink
OBCLIB = $(OBCDIR)/lib/obc

OBJS = MyFiles.k Strings1.k Fifo.k b.k Complete.k Testbed.k

all: xml2pmx

update: ../image.c

xml2pmx: $(OBJS)
	$(OBC) $^ -o $@

../image.c: $(OBJS) force
	$(OBLINK) -g -pl -L $(OBCLIB) $(OBJS) -C -p -o $@ >../primtab0.c

%.k: %.m
	$(OBC) -c -x $< -o $@

clean: force
	rm -f *.k

force:

###

b.k: Strings1.k Fifo.k MyFiles.k
Complete.k: Strings1.k MyFiles.k
Testbed.k: Strings1.k b.k Complete.k MyFiles.k
