# this makefile is for release and installation purposes only

# Installation-related stuff
#
# Make sure that you are installing in the same hierarchy as ttf2pt1 itself!
# 
# The base dir for installation and subdirs in it
INSTDIR = /usr/local
# for scripts, maps/encodings etc.
SHAREDIR = $(INSTDIR)/share/ttf2pt1

# owner and group of installed files
OWNER = root
GROUP = bin

all:

release:
	./mkrel release

snapshot:
	./mkrel snapshot

install:
	cp -p *.map $(SHAREDIR)/maps
	chown $(OWNER) $(SHAREDIR)/maps/*.map
	chgrp $(GROUP) $(SHAREDIR)/maps/*.map
	chmod 0644 $(SHAREDIR)/maps/*.map

uninstall:
	for i in *.map; do rm -f $(SHAREDIR)/maps/$$i || exit 1; done
