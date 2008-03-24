## luatexlib.mk - Makefile fragment for libraries used by pdf[ex]tex.
# $Id$
# Public domain.

# The pdf*tex programs depend on a number of libraries.
# Include dependencies to get the built if we don't do make
# from the top-level directory.

Makefile: luatexdir/luatexlib.mk

# libz

ZLIBDIR=../../libs/zlib
ZLIBSRCDIR=$(srcdir)/$(ZLIBDIR)
ZLIBDEP = @ZLIBDEP@
LDZLIB = @LDZLIB@

$(ZLIBDIR)/libz.a: $(ZLIBSRCDIR)
	cd $(ZLIBDIR) && $(MAKE) $(common_makeargs) libz.a

# libpng

LIBPNGDIR=../../libs/libpng
LIBPNGSRCDIR=$(srcdir)/$(LIBPNGDIR)
LIBPNGDEP = @LIBPNGDEP@
LDLIBPNG = @LDLIBPNG@

$(LIBPNGDIR)/libpng.a: $(LIBPNGSRCDIR)/*.c
	cd $(LIBPNGDIR) && $(MAKE) $(common_makeargs) libpng.a


# libxpdf

LIBXPDFDIR=../../libs/xpdf
LIBXPDFSRCDIR=$(srcdir)/$(LIBXPDFDIR)
LIBXPDFDEP = @LIBXPDFDEP@
LDLIBXPDF = @LDLIBXPDF@

$(LIBXPDFDIR)/fofi/libfofi.a: $(LIBXPDFSRCDIR)/fofi/*.cc \
	$(LIBXPDFSRCDIR)/fofi/*.h
	cd $(LIBXPDFDIR)/fofi; $(MAKE) $(common_makeargs) libfofi.a
$(LIBXPDFDIR)/goo/libGoo.a: $(LIBXPDFSRCDIR)/goo/*.cc \
	$(LIBXPDFSRCDIR)/goo/*.c $(LIBXPDFSRCDIR)/goo/*.h
	cd $(LIBXPDFDIR)/goo; $(MAKE) $(common_makeargs) libGoo.a
$(LIBXPDFDIR)/xpdf/libxpdf.a: $(LIBXPDFSRCDIR)/xpdf/*.cc \
	$(LIBXPDFSRCDIR)/xpdf/*.h
	cd $(LIBXPDFDIR)/xpdf; $(MAKE) $(common_makeargs) libxpdf.a


# md5

LIBMD5DIR=../../libs/md5
LIBMD5SRCDIR=$(srcdir)/$(LIBMD5DIR)
LIBMD5DEP=$(LIBMD5DIR)/md5.o

$(LIBMD5DEP): $(LIBMD5SRCDIR)/md5.c $(LIBMD5SRCDIR)/md5.h
clean:: md5lib-clean
md5lib-clean:
	rm -f $(LIBMD5DEP)

# obsdcompat
LIBOBSDDIR=../../libs/obsdcompat
LIBOBSDSRCDIR=$(srcdir)/$(LIBOBSDCOMPATDIR)
LIBOBSDDEP=@LIBOBSDDEP@
LDLIBOBSD=@LDLIBOBSD@

$(LIBOBSDDIR)/libopenbsd-compat.a: $(LIBOBSDSRCDIR)/*.c $(LIBOBSDSRCDIR)/*.h
# common_makeargs = $(MFLAGS) CC='$(CC)' CFLAGS='$(CFLAGS)' LDFLAGS='$(LDFLAGS)' $(XMAKEARGS)
# CFLAGS setzt libopenbsd-compat selbst, nicht durchreichen!
	cd $(LIBOBSDDIR); $(MAKE) $(MFLAGS) $(XMAKEARGS) libopenbsd-compat.a

# libpdf itself
pdflib = luatexdir/libpdf.a
pdflib_sources = $(srcdir)/luatexdir/*.c $(srcdir)/luatexdir/*.cc \
	$(srcdir)/luatexdir/*.h

luatexdir/libpdf.a: $(pdflib_sources) luatexdir/luatexextra.h ctangle
	cd luatexdir && mkdir -p font image lua lang tex && $(MAKE) $(common_makeargs) libpdf.a

# makecpool:

luatexdir/makecpool: luatexdir/makecpool.c
	cd luatexdir && $(MAKE) $(common_makeargs) makecpool

# lua

LIBLUADIR=../../libs/lua51
LIBLUASRCDIR=$(srcdir)/$(LIBLUADIR)
LIBLUADEP=$(LIBLUADIR)/liblua.a

luatarget=posix
ifeq ($(target),i386-mingw32)
  ifeq ($(host),i386-linux)
    luatarget = mingwcross
  else
    luatarget = mingw
  endif
else
ifeq ($(target),i386-linux)
  luatarget = posix
endif
endif


$(LIBLUADEP):
	mkdir -p $(LIBLUADIR) && cd $(LIBLUADIR) && cp -f $(LIBLUASRCDIR)/* . && $(MAKE) $(luatarget)

# slnunicode
SLNUNICODEDIR=../../libs/slnunicode
SLNUNICODESRCDIR=$(srcdir)/$(SLNUNICODEDIR)
SLNUNICODEDEP=$(SLNUNICODEDIR)/slnunico.o
$(SLNUNICODEDEP): $(SLNUNICODEDIR)/slnunico.c $(SLNUNICODEDIR)/slnudata.c
	mkdir -p $(SLNUNICODEDIR) && cd $(SLNUNICODEDIR) && cp -f $(SLNUNICODESRCDIR)/* . && $(CC) $(CFLAGS) -I$(LIBLUADIR) -o slnunico.o -c slnunico.c

# zziplib

# zziplib is a configuration nightmare, because it is so stubborn
# about using a pre-installed zlib. configure and make have to
# be fooled in two different ways. 
# - configure needs to find
# the subdirs 'include' and 'lib' in the --with-zlib prefix,
# and these need to contain the proper headers and libraries.
# - make needs to find the right headers using an -I via CPPFLAGS, 
# because the zlib path is relative and can't be found otherwise.
#
# final trickyness: configure writes a Makefile in the source
# directory. it is needed by anything, so I delete it right away

zzipretarget=

ifeq ($(target),i386-mingw32)
  zzipretarget=--target=$(target) --build=$(target) --host=$(host)
endif


ZZIPLIBDIR=../../libs/zziplib
ZZIPLIBSRCDIR=$(srcdir)/$(ZZIPLIBDIR)
ZZIPLIBDEP = $(ZZIPLIBDIR)/zzip/libzzip.a

$(ZZIPLIBDEP): $(ZZIPLIBSRCDIR)
	mkdir -p $(ZZIPLIBDIR)/zzip && cd $(ZZIPLIBDIR)/zzip && \
	cp ../$(ZZIPLIBSRCDIR)/zzip/Makefile . && $(MAKE) $(common_makeargs)

# luazip

LUAZIPDIR=../../libs/luazip
LUAZIPSRCDIR=$(srcdir)/$(LUAZIPDIR)
LUAZIPDEP=$(LUAZIPDIR)/src/luazip.o
LUAZIPINC=-I../../lua51 -I../$(ZZIPLIBSRCDIR) -I../$(ZZIPLIBDIR)

$(LUAZIPDEP): $(LUAZIPDIR)/src/luazip.c
	mkdir -p $(LUAZIPDIR) && cd $(LUAZIPDIR) && cp -R $(LUAZIPSRCDIR)/* . && \
    cd src && $(CC) $(CFLAGS) $(LUAZIPINC) -g -o luazip.o -c luazip.c

# luafilesystem

LUAFSDIR=../../libs/luafilesystem
LUAFSSRCDIR=$(srcdir)/$(LUAFSDIR)
LUAFSDEP=$(LUAFSDIR)/src/lfs.o
LUAFSINC=-I../../lua51

$(LUAFSDEP): $(LUAFSDIR)/src/lfs.c $(LUAFSDIR)/src/lfs.h
	mkdir -p $(LUAFSDIR) && cd $(LUAFSDIR) && cp -R $(LUAFSSRCDIR)/* . && \
    cd src && $(CC) $(CFLAGS) $(LUAFSINC) -g -o lfs.o -c lfs.c

# luapeg
LUAPEGDIR=../../libs/luapeg
LUAPEGSRCDIR=$(srcdir)/$(LUAPEGDIR)
LUAPEGDEP=$(LUAPEGDIR)/lpeg.o
$(LUAPEGDEP): $(LUAPEGDIR)/lpeg.c
	mkdir -p $(LUAPEGDIR) && cd $(LUAPEGDIR) && cp -f $(LUAPEGSRCDIR)/* . && $(CC) $(CFLAGS) -I$(LIBLUADIR) -g -o lpeg.o -c lpeg.c


# luamd5
LUAMDVDIR=../../libs/luamd5
LUAMDVSRCDIR=$(srcdir)/$(LUAMDVDIR)
LUAMDVDEP=$(LUAMDVDIR)/md5lib.o $(LUAMDVDIR)/md5.o
$(LUAMDVDEP): $(LUAMDVDIR)/md5lib.c $(LUAMDVDIR)/md5.h $(LUAMDVDIR)/md5.c
	mkdir -p $(LUAMDVDIR) && cd $(LUAMDVDIR) && cp -f $(LUAMDVSRCDIR)/* . && $(CC) $(CFLAGS) -I$(LIBLUADIR) -g -o md5.o -c md5.c && $(CC) $(CFLAGS) -I$(LIBLUADIR) -g -o md5lib.o -c md5lib.c

.PHONY: always

# luaff
LUAFFDIR=../../libs/luafontforge
LUAFFSRCDIR=$(srcdir)/$(LUAFFDIR)
LUAFFDEP=$(LUAFFDIR)/libff.a
$(LUAFFDEP): always
	mkdir -p $(LUAFFDIR) && cp -f $(LUAFFSRCDIR)/Makefile $(LUAFFDIR)
	mkdir -p $(LUAFFDIR)/fontforge && cp -f $(LUAFFSRCDIR)/fontforge/fontforge/Makefile $(LUAFFDIR)/fontforge
	mkdir -p $(LUAFFDIR)/Unicode && cp -f $(LUAFFSRCDIR)/fontforge/Unicode/Makefile $(LUAFFDIR)/Unicode
	cd $(LUAFFDIR) && $(MAKE)


# luazlib
LUAZLIBDIR=../../libs/luazlib
LUAZLIBSRCDIR=$(srcdir)/$(LUAZLIBDIR)
LUAZLIBDEP=$(LUAZLIBDIR)/lgzip.o $(LUAZLIBDIR)/lzlib.o
LUAZLIBINC=-I$(ZLIBSRCDIR) -I$(LIBLUASRCDIR)
$(LUAZLIBDEP): $(LUAZLIBDIR)/lgzip.c $(LUAZLIBDIR)/lzlib.c
	mkdir -p $(LUAZLIBDIR) && cd $(LUAZLIBDIR) && cp -f $(LUAZLIBSRCDIR)/* . && $(CC) $(CFLAGS) $(LUAZLIBINC) -g -o lgzip.o -c lgzip.c && $(CC) $(CFLAGS) $(LUAZLIBINC) -g -o lzlib.o -c lzlib.c


# Convenience variables.

luatexlibs = $(pdflib) $(LDLIBPNG) $(LDZLIB) $(LDLIBXPDF) $(LIBMD5DEP) $(LDLIBOBSD) \
             $(LIBLUADEP) $(SLNUNICODEDEP)  $(LUAZIPDEP) $(ZZIPLIBDEP) $(LUAFSDEP) \
             $(LUAPEGDEP) $(LUAMDVDEP)  $(LUAZLIBDEP) $(LUAFFDEP)

luatexlibsdep = $(pdflib) $(LIBPNGDEP) $(ZLIBDEP) $(LIBXPDFDEP) $(LIBMD5DEP) $(LIBOBSDDEP) \
                $(LIBLUADEP) $(SLNUNICODEDEP) $(ZZIPLIBDEP) $(LUAZIPDEP)  $(LUAFSDEP) \
                $(LUAPEGDEP) $(LUAMDVDEP)  $(LUAZLIBDEP) $(LUAFFDEP) $(makecpool)

## end of luatexlib.mk - Makefile fragment for libraries used by pdf[ex]tex.

