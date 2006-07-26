# SFconv - ultra quick-and-dirty piece of code....

expat_o = expat/xmlparse/xmlparse.o	\
	expat/xmlparse/hashtable.o	\
	expat/xmltok/xmlrole.o \
	expat/xmltok/xmltok.o

objs = SFconv.o ConvertUTF.o $(expat_o)

libs = -lstdc++ -lgcc -lc

CFLAGS = -I../source/Public-headers -I./expat/xmlparse -I./expat/xmltok -DXML_DTD

sfconv: $(objs) /usr/local/lib/TECkit.dylib
	$(CC) $(CFLAGS) -o $@ $(objs) /usr/local/lib/TECkit.dylib $(libs)

SFconv.o: SFconv.cpp sfReader.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

install: sfconv
	cp sfconv /usr/local/bin/

clean:
	rm -f $(objs) sfconv
