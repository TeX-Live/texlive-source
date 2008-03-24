# $Id$

srcdir=../../../src/libs/luafontforge

FF_LIB=ffdummies.o luafflib.o

MY_CFLAGS=-g -O2 -Wall -DFONTFORGE_CONFIG_NO_WINDOWING_UI=1 -DLUA_FF_LIB=1 $(CFLAGS) $(XCFLAGS)

.PHONY: newfile

fontforge: $(FF_LIB) newfile
	rm -f libff.a
	cd fontforge && $(MAKE)
	cd Unicode && $(MAKE)
	sh -c "ar cru libff.a fontforge/*.o Unicode/*.o $(FF_LIB)"
	ranlib libff.a

%.o: $(srcdir)/src/%.c
	gcc $(MY_CFLAGS) -I$(srcdir)/../lua51 -I$(srcdir)/fontforge/inc -I$(srcdir)/fontforge/fontforge -c $< -o $@

clean:
	rm  -f libff.a $(FF_LIB)
	cd fontforge; $(MAKE) clean
	cd Unicode; $(MAKE) clean