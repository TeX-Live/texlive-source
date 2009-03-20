##
INCLUDES = $(X_CFLAGS)

LDADD = $(X_LIBS) -lX11 -lXmu $(X_EXTRA_LIBS)

bin_PROGRAMS = pdfopen pdfclose

pdfopen_SOURCES = pdfopen.c sendx.c

pdfclose_SOURCES = pdfclose.c sendx.c

EXTRA_DIST = xpdfopen.pdf xpdfopen.tex

## Eventually delete these files
##
EXTRA_DIST += Makefile.am.orig configure.ac.orig

