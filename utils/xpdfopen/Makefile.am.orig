
AM_CFLAGS = $(X_CFLAGS)
## Add warnings for gcc-3.4
AM_CFLAGS += -Wall -Wunused -Wimplicit -Wreturn-type
AM_CFLAGS += -Wdeclaration-after-statement -Wno-unknown-pragmas
AM_CFLAGS += -Wmissing-prototypes -Wmissing-declarations -Wparentheses
AM_CFLAGS += -Wswitch -Wtrigraphs -Wpointer-arith -Wcast-qual
AM_CFLAGS += -Wcast-align -Wwrite-strings -Wold-style-definition

LDADD = $(X_LIBS) -lX11 -lXmu $(X_EXTRA_LIBS)

bin_PROGRAMS = pdfopen pdfclose

pdfopen_SOURCES = externs.h pdfopen.c sendx.c utils.c sendx.h xpdfopen.h utils.h

pdfclose_SOURCES = externs.h pdfclose.c sendx.c utils.c sendx.h xpdfopen.h utils.h


