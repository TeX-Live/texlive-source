#!/bin/sh
#
# Attatch patch for ptex-src-3.1.11 under TeX Live 2009.
#

echo ${PATCH:=patch} > /dev/null
echo ${CP:=cp -fp}   > /dev/null
echo ${RM:=rm -f}    > /dev/null
echo ${MV:=mv -f}    > /dev/null

## check

if test ! -d kpathsea; then
    cat <<-EOF

	This script should be executed under 'texk'.
	Excute as following.

	  cd /your/path/to/texk
	  ./ptexenc/patches/texlive2009.sh

EOF
    exit 1
fi

if grep "2009 TeX Users Group" ../README > /dev/null; then
    echo "dummy" > /dev/null
else
    echo "This is for TeX Live 2009."
    exit 1
fi

## for ptexenc

$PATCH -p1 -d .. < ptexenc/patches/texlive2009-ptexenc.patch || exit

## for ptex

(cd web2c/ptexdir
mkdir lib am
$RM Makefile.in ptexextra.c version.c mkconf pconvert configure \
    jbibextra.* jbibd.sed usage.c
sed -e s/etex/ptex/g -e s/@ETEX@/@JPTEX@/g ../etexdir/etex.mk > ptex.mk
sed -e 's/e-*tex/ptex/g' -e 's/e-*TeX/pTeX/g' -e s/etrip/ptrip/g \
    ../etexdir/am/etex.am > am/ptex.am
$CP ../lib/openclose.c lib
$CP ../lib/printversion.c lib
$CP ../lib/usage.c lib
$MV kanji.h.in kanji.h
$MV kanji.defines ptex.defines
$MV ptexextra.h ptexextra.in
$MV jbibtex.ch pbibtex.ch
$MV jbibtex.defines pbibtex.defines
$MV pltotf.ch ppltotf.ch
$MV tftopl.ch ptftopl.ch
)

$PATCH -p1 -d web2c/ptexdir < ptexenc/patches/ptex-src-3.1.11-ptexenc.patch || exit

## for mendex
if test -d mendexk; then
    $CP makeindexk/configure   mendexk/
    $CP makeindexk/Makefile.in mendexk/
    $CP makeindexk/Makefile.am mendexk/
    $CP makeindexk/aclocal.m4  mendexk/
    $PATCH -p1 -d mendexk < ptexenc/patches/mendexk2.6f-ptexenc.patch || exit
fi

## for makejvf
if test -d makejvf; then
    $CP makeindexk/configure   makejvf/
    $CP makeindexk/Makefile.in makejvf/
    $CP makeindexk/Makefile.am makejvf/
    $CP makeindexk/aclocal.m4  makejvf/
    $PATCH -p1 -d makejvf < ptexenc/patches/makejvf-1.1a-ptexenc.patch || exit
fi
