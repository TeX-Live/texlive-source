FixedPoint.o: $(srcdir)/FixedPoint.cc ../aconf.h \
  $(srcdirparent)/aconf2.h
gfile.o: $(srcdir)/gfile.cc ../aconf.h \
  $(srcdirparent)/aconf2.h \
  $(srcdir)/GString.h \
  $(srcdir)/gfile.h \
  $(srcdir)/gtypes.h
GHash.o: $(srcdir)/GHash.cc ../aconf.h \
  $(srcdirparent)/aconf2.h \
  $(srcdir)/gmem.h \
  $(srcdir)/GString.h \
  $(srcdir)/GHash.h \
  $(srcdir)/gtypes.h
GList.o: $(srcdir)/GList.cc ../aconf.h \
  $(srcdirparent)/aconf2.h \
  $(srcdir)/gmem.h \
  $(srcdir)/GList.h \
  $(srcdir)/gtypes.h
gmempp.o: $(srcdir)/gmempp.cc ../aconf.h \
  $(srcdirparent)/aconf2.h \
  $(srcdir)/gmem.h
GString.o: $(srcdir)/GString.cc ../aconf.h \
  $(srcdirparent)/aconf2.h \
  $(srcdir)/gtypes.h \
  $(srcdir)/GString.h
gmem.o: $(srcdir)/gmem.c ../aconf.h \
  $(srcdirparent)/aconf2.h \
  $(srcdir)/gmem.h
parseargs.o: $(srcdir)/parseargs.c \
  $(srcdir)/parseargs.h \
  $(srcdir)/gtypes.h
