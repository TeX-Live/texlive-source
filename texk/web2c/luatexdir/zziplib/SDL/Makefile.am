AUTOMAKE_OPTIONS = 1.4 foreign
AUTOTOOL_VERSION=autoconf-2.52 automake-1.5 libtool-1.4.2

sdldir = ${pkgdatadir}/SDL
sdl_DATA = SDL_rwops_zzip.c SDL_rwops_zzip.h

EXTRA_DIST = $(sdl_DATA)
check_PROGRAMS = zzcatsdl

zzcatsdl_SOURCES = SDL_rwops_zzcat.c
zzcatsdl_LDFLAGS = `sdl-config --libs`   `./zzip-config --libs` 
zzcatsdl_cflags  = `sdl-config --cflags` `./zzip-config --cflags`

SDL_rwops_zzip.$(OBJEXT) :  zzip-config  @srcdir@/SDL_rwops_zzip.c
	$(COMPILE) $(zzcatsdl_cflags) -c @srcdir@/SDL_rwops_zzip.c  -o $@ 
SDL_rwops_zzcat.$(OBJEXT) : zzip-config  @srcdir@/SDL_rwops_zzcat.c
	$(COMPILE) $(zzcatsdl_cflags) -c @srcdir@/SDL_rwops_zzcat.c -o $@
zzcatsdl$(EXEEXT): SDL_rwops_zzip.$(OBJEXT) SDL_rwops_zzcat.$(OBJEXT)
	$(LINK)    SDL_rwops_zzip.$(OBJEXT) SDL_rwops_zzcat.$(OBJEXT) \
	           $(zzcatsdl_LDFLAGS)  -o $@

zzip-config : ../bins/zzip-config
	sed -e 's:includedir=.*:includedir=..:' \
	    -e 's:libdir=.*:libdir=../zzip:'  $? > $@
	chmod +x $@

test.zip : @top_srcdir@/test/test.zip
	$(LN_S) $? $@
testsdl test-sdl : test.zip zzcatsdl$(EXEEXT)
	./zzcatsdl$(EXEEXT) test/README > test.out
	diff -q @top_srcdir@/README test.out && rm test.out

sdl : all
install-sdl : install-data # and not the binary

### if it wouldn't be for the zzip-config then we could write it like this
# zzcatsdl_SOURCES = SDL_rwops_zzcat.c SDL_rwops_zzip.c
# zzcatsdl_LDADD   = `sdl-config --libs`   `zzip-config --libs` 
# CFLAGS  = `sdl-config --cflags` `zzip-config --cflags`
###

CLEANFILES = *.pc *.omf
# ----------------------------------------------------------------------
# we do not install a library, so this about equivalent to some SDL.pc
pkgconfigdir=$(libdir)/pkgconfig
pkgconfig_HEADERS= zzip-sdl-config.pc zzip-sdl-rwops.pc
ERRNULL= 2>/dev/null
zzip-sdl-config.pc : Makefile
	@ echo "prefix="`sdl-config --prefix $(ERRNULL)` >$@
	@ echo "sdl_static_libs="`sdl-config --static-libs $(ERRNULL)` >>$@
	@ echo "sdl_libs="`sdl-config --libs $(ERRNULL)` >>$@
	@ echo "sdl_cflags="`sdl-config --cflags $(ERRNULL)` >>$@
	@ echo " " >>$@
	@ echo "Name: zzip-sdl-config" >>$@
	@ echo "Version: "`sdl-config --version $(ERRNULL)` >>$@
	@ echo "Description: SDL Config (for ZZipLib)" >>$@
	echo 'Libs: $${sdl_libs}' >>$@
	echo 'Cflags: $${sdl_cflags}' >>$@

zzip-sdl-rwops.pc : Makefile
	@ echo 'zzipdatadir=$(pkgdatadir)' >$@
	@ echo 'zzipsdlrwops=$${zzipdatadir}/SDL' >>$@
	@ echo " " >>$@
	@ echo "Name: zzip-sdl-rwops" >>$@
	@ echo "Version: $(VERSION)" >>$@
	@ echo "Description: SDL_rwops for ZZipLib" >>$@
	echo 'Requires: zzip-sdl-config, zziplib' >>$@
	echo 'Cflags: -I$${zzipsdlrwops}' >>$@
