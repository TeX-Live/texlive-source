
ifeq ($(ARCH),)
	ARCH=x64
endif

UNAME:=$(shell uname)
ifeq ($(UNAME),Linux)
	BUILDSUFFIX=_ux_$(ARCH)
	SOEXT=.so
	EXEEXT=
#	SHAREDCOPTS=-fPIC
	LFLAGS=
	LIBS=-lm
else
	BUILDSUFFIX=_win_$(ARCH)
	SOEXT=.dll
	EXEEXT=.exe
#	SHAREDCOPTS=-DPPDLL
	LFLAGS = -Wl,--output-def,$@.def,--out-implib,$@.a
	LIBS=
endif

CC=gcc
WLUIGI=-Wall -Wextra -Wunused -Wimplicit -Wreturn-type -Wdeclaration-after-statement -Wno-unknown-pragmas -Wmissing-prototypes -Wmissing-declarations -Wparentheses -Wswitch -Wtrigraphs -Wcast-qual -Wcast-align -Wwrite-strings -Wold-style-definition -Wpointer-arith
WHANS=-Wall -Wextra -Wcast-align -Wcast-qual -Wnested-externs -Wwrite-strings -Wdeclaration-after-statement -Wno-unknown-pragmas -Wpointer-arith
W=$(WLUIGI)

CFLAGS=$(W) -O3

OBJEXT=$(BUILDSUFFIX).o

PPALLOC=utilmem utilmemheap utilmemheapiof utilmeminfo
PPUTILS=utillog utilnumber utiliof utilmd5 utilsha utilcrypt
PPFILTERS=utilbasexx utilflate utillzw utilfpred
PPMODULES=ppheap pparray ppdict ppstream ppcrypt ppxref ppload

OUTDIR=build
UTILSDIR=util

PPSTATICALLOCOBJ=$(patsubst %, $(OUTDIR)/%$(OBJEXT), $(PPALLOC))
PPSTATICUTILSOBJ=$(patsubst %, $(OUTDIR)/%$(OBJEXT), $(PPUTILS))
PPSTATICFILTERSOBJ=$(patsubst %, $(OUTDIR)/%$(OBJEXT), $(PPFILTERS))
PPSTATICOBJ=$(patsubst %, $(OUTDIR)/%$(OBJEXT), $(PPMODULES))
PPSTATICLIB=$(OUTDIR)/libpp.static$(BUILDSUFFIX).a

# dependencies
FLATEDIR=../zlib
FLATELIB=$(FLATEDIR)/libz.static$(BUILDSUFFIX).a
PPSTATICDEPS=$(FLATELIB)

# tests
TESTDIR=../../pplib-tests
PPTEST1=$(OUTDIR)/pptest1$(EXEEXT)
PPTEST2=$(OUTDIR)/pptest2$(EXEEXT)
PPTEST3=$(OUTDIR)/pptest3$(EXEEXT)

# includes
INC=-I $(UTILSDIR)
AUX=-I $(FLATEDIR)

default: md $(PPSTATICLIB) $(PPTEST1) $(PPTEST2) $(PPTEST3) $(PPTEST4)

md:
	@[ -d $(OUTDIR) ] || mkdir -p $(OUTDIR)

$(PPSTATICOBJ): $(OUTDIR)/%$(OBJEXT): %.c %.h
	$(CC) $(CFLAGS) $(INC) -o $@ -c $<

$(PPSTATICALLOCOBJ): $(OUTDIR)/%$(OBJEXT): $(UTILSDIR)/%.c $(UTILSDIR)/%.h
	$(CC) $(CFLAGS) $(INC) -o $@ -c $<

$(PPSTATICUTILSOBJ): $(OUTDIR)/%$(OBJEXT): $(UTILSDIR)/%.c $(UTILSDIR)/%.h
	$(CC) $(CFLAGS) $(INC) -o $@ -c $<

$(PPSTATICFILTERSOBJ): $(OUTDIR)/%$(OBJEXT): $(UTILSDIR)/%.c $(UTILSDIR)/%.h
	$(CC) $(CFLAGS) $(INC) $(AUX) -o $@ -c $<

$(PPSTATICLIB): $(PPSTATICOBJ) $(PPSTATICALLOCOBJ) $(PPSTATICUTILSOBJ) $(PPSTATICFILTERSOBJ)
	ar rcs $@ $^

$(PPTEST1): $(PPSTATICLIB) pptest1.c
	$(CC) $(CFLAGS) -o $(OUTDIR)/pptest1$(OBJEXT) -c pptest1.c
	$(CC) -static-libgcc $(CFLAGS) -o $(PPTEST1) $(OUTDIR)/pptest1$(OBJEXT) $(PPSTATICLIB) $(PPSTATICDEPS) $(LIBS)

$(PPTEST2): $(PPSTATICLIB) pptest2.c
	$(CC) $(CFLAGS) -o $(OUTDIR)/pptest2$(OBJEXT) -c pptest2.c
	$(CC) -static-libgcc $(CFLAGS) -o $(PPTEST2) $(OUTDIR)/pptest2$(OBJEXT) $(PPSTATICLIB) $(PPSTATICDEPS) $(LIBS)

$(PPTEST3): $(PPSTATICLIB) pptest3.c
	$(CC) $(CFLAGS) -I util -o $(OUTDIR)/pptest3$(OBJEXT) -c pptest3.c
	$(CC) -static-libgcc $(CFLAGS) -o $(PPTEST3) $(OUTDIR)/pptest3$(OBJEXT) $(PPSTATICLIB) $(PPSTATICDEPS) $(LIBS)

clean:
	rm -f $(OUTDIR)/*$(OBJEXT) $(OUTDIR)/*.a $(OUTDIR)/*$(EXEEXT)

test: $(PPTEST1) $(PPTEST2) $(PPTEST3)
	build/pptest1 $(TESTDIR)/*.pdf $(TESTDIR)/crypt/*.pdf >pptest1.log
	build/pptest2 $(TESTDIR)/*.pdf $(TESTDIR)/crypt/*.pdf >pptest2.log
	build/pptest3 $(TESTDIR)/*.pdf $(TESTDIR)/crypt/*.pdf >pptest3.log
