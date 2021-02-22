#! /usr/bin/gmake -f

# the 'all' target is included from the 'configure'd Makefile

default:
	@ test -f Makefile || test -d build || (set -x ; mkdir build ; cd build && sh ../configure --prefix=$$HOME)
	@ test -f Makefile || test ! -f build/Makefile || (set -x ; cd build && $(MAKE) all)
	@ test -f Makefile || test ! -f build/Makefile || echo 'DONE (cd build && make all) - please run (cd build && make check) now'
	@ test ! -f Makefile || test -f build/Makefile || $(MAKE) all
	@ test ! -f Makefile || test -f build/Makefile || echo 'DONE make all - please run make check (before make install)'

cm cmake: ; rm -rf build; mkdir build; cd build && cmake .. -DCMAKE_INSTALL_PREFIX:PATH=$$HOME/local
am autom: ; rm -rf build2; mkdir build2; cd build2 && sh ../configure --prefix=$$HOME/local --enable-sdl
2: ; cd build2 && $(MAKE) all

new: ; rm -rf build; $(MAKE) default

auto:
	aclocal -I m4 && autoconf -I m4 && autoheader && automake

boottrap:
	rm -rf .deps .libs
	rm -f config.guess config.sub stamp-h.in
	rm -f install-sh ltconfig ltmain.sh depcomp mkinstalldirs
	rm -f config.h config.h.in config.log config.cache configure
	rm -f aclocal.m4 Makefile Makefile.in
	aclocal 
	autoconf 
	autoheader 
	automake -a -c 

-include Makefile

test_%: ; python3 testbuilds.py $@ -vv
tests:  ; python3 testbuilds.py -vv

version:
	oldv=`sed -e '/zziplib.VERSION/!d' -e 's:.*zziplib.VERSION."::' -e 's:".*::' CMakeLists.txt` \
	; oldr=`echo $$oldv | sed -e 's:.*[.]::'` ; newr=`expr $$oldr + 1` \
	; newv=`echo $$oldv | sed -e "s:[.]$$oldr\$$:.$$newr:"` \
	; echo "$$oldv -> $$newv" \
	; sed -i -e "s:$$oldv:$$newv:" zziplib.spec testbuilds.py \
	; sed -i -e "s:$$oldv:$$newv:" */CMakeLists.txt \
	; sed -i -e "s:$$oldv:$$newv:" CMakeLists.txt \
	; git diff -U0
