## luatexlib.mk - Makefile fragment for libraries used by luatex.
# $Id$
# Public domain.

# luatex depends on a number of libraries.
# Include dependencies to get them built if we don't do make
# from the top-level directory.

Makefile: luatexdir/luatexlib.mk

# libpdf itself
luapdflib = luatexdir/libpdf.a
luapdflib_sources = $(srcdir)/luatexdir/*.c $(srcdir)/luatexdir/*.h \
	$(srcdir)/luatexdir/*/*.c $(srcdir)/luatexdir/*/*.h \
	$(srcdir)/luatexdir/*/*.cc

$(luapdflib): $(luapdflib_sources) luatexdir/luatexextra.h ctangle
	cd luatexdir && mkdir -p font image lua lang pdf tex utils && $(MAKE) $(common_makeargs) libpdf.a

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


# luasocket

LUASOCKETDIR=../../libs/luasocket
LUASOCKETSRCDIR=$(srcdir)/$(LUASOCKETDIR)
LUASOCKETDEP=$(LUASOCKETDIR)/src/socket.a
LUASOCKETINC=-I../../lua51

$(LUASOCKETDEP): $(LUASOCKETDIR)/src/luasocket.c $(LUASOCKETDIR)/src/luasocket.h
	mkdir -p $(LUASOCKETDIR) && cd $(LUASOCKETDIR) && cp -R $(LUASOCKETSRCDIR)/* . && \
    cd src && make


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
	mkdir -p $(LUAFFDIR) && cd $(LUAFFDIR) && cp -f $(LUAFFSRCDIR)/Makefile . && $(MAKE)

# luaprof
LUAPROFDIR=../../libs/luaprofiler
LUAPROFSRCDIR=$(srcdir)/$(LUAPROFDIR)
LUAPROFDEP=$(LUAPROFDIR)/libprofiler.a
$(LUAPROFDEP): always
	mkdir -p $(LUAPROFDIR) && cd $(LUAPROFDIR) && cp -f $(LUAPROFSRCDIR)/Makefile . && $(MAKE)


# luazlib
LUAZLIBDIR=../../libs/luazlib
LUAZLIBSRCDIR=$(srcdir)/$(LUAZLIBDIR)
LUAZLIBDEP=$(LUAZLIBDIR)/lgzip.o $(LUAZLIBDIR)/lzlib.o
LUAZLIBINC=-I$(ZLIBSRCDIR) -I$(LIBLUASRCDIR)
$(LUAZLIBDEP): $(LUAZLIBDIR)/lgzip.c $(LUAZLIBDIR)/lzlib.c
	mkdir -p $(LUAZLIBDIR) && cd $(LUAZLIBDIR) && cp -f $(LUAZLIBSRCDIR)/* . && $(CC) $(CFLAGS) $(LUAZLIBINC) -g -o lgzip.o -c lgzip.c && $(CC) $(CFLAGS) $(LUAZLIBINC) -g -o lzlib.o -c lzlib.c

# mplib (temporary)

LIBMPDIR=mpdir
LIBMPLIBDEP=$(LIBMPDIR)/lmplib.o $(LIBMPDIR)/.libs/libmplib.a

# Convenience variables.

luatexlibs = $(luapdflib) $(LDLIBPNG) $(LDZLIB) $(LDLIBXPDF) $(LIBMD5DEP) $(LDLIBOBSD) \
             $(LIBLUADEP) $(SLNUNICODEDEP)  $(LUAZIPDEP) $(ZZIPLIBDEP) $(LUAFSDEP) $(LUASOCKETDEP) \
             $(LUAPEGDEP) $(LUAMDVDEP)  $(LUAZLIBDEP) $(LUAFFDEP) $(LUAPROFDEP) $(LIBMPLIBDEP)

luatexlibsdep = $(luapdflib) $(LIBPNGDEP) $(ZLIBDEP) $(LIBXPDFDEP) $(LIBMD5DEP) $(LIBOBSDDEP) \
                $(LIBLUADEP) $(SLNUNICODEDEP) $(ZZIPLIBDEP) $(LUAZIPDEP)  $(LUAFSDEP) $(LUASOCKETDEP) \
                $(LUAPEGDEP) $(LUAMDVDEP)  $(LUAZLIBDEP) $(LUAFFDEP) $(LUAPROFDEP) $(makecpool)

## end of luatexlib.mk - Makefile fragment for libraries used by pdf[ex]tex.

