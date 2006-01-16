FoFiBase.o: $(srcdir)/FoFiBase.cc ../aconf.h \
  $(srcdirparent)/aconf2.h \
  $(goodir)/gmem.h \
  $(srcdir)/FoFiBase.h \
  $(goodir)/gtypes.h
FoFiEncodings.o: $(srcdir)/FoFiEncodings.cc \
  ../aconf.h $(srcdirparent)/aconf2.h \
  $(srcdir)/FoFiEncodings.h \
  $(goodir)/gtypes.h
FoFiTrueType.o: $(srcdir)/FoFiTrueType.cc \
  ../aconf.h $(srcdirparent)/aconf2.h \
  $(goodir)/gtypes.h \
  $(goodir)/gmem.h \
  $(goodir)/GString.h \
  $(goodir)/GHash.h \
  $(srcdir)/FoFiTrueType.h \
  $(srcdir)/FoFiBase.h
FoFiType1.o: $(srcdir)/FoFiType1.cc ../aconf.h \
  $(srcdirparent)/aconf2.h \
  $(goodir)/gmem.h \
  $(srcdir)/FoFiEncodings.h \
  $(goodir)/gtypes.h \
  $(srcdir)/FoFiType1.h \
  $(srcdir)/FoFiBase.h
FoFiType1C.o: $(srcdir)/FoFiType1C.cc ../aconf.h \
  $(srcdirparent)/aconf2.h \
  $(goodir)/gmem.h \
  $(goodir)/GString.h \
  $(srcdir)/FoFiEncodings.h \
  $(goodir)/gtypes.h \
  $(srcdir)/FoFiType1C.h \
  $(srcdir)/FoFiBase.h
