#!/bin/sh -l

set -e

if [ "x$2" = "x" ]
then
  echo "Usage: `basename $0` arch buildsys [no-prepare]" >&2
  exit 1
fi

arch="$1"
echo "Building TL asy for arch = $arch"
shift

buildsys=$1
echo "Building on $buildsys"
shift

do_prepare=1
if [ "$1" = "no-prepare" ]
then
  do_prepare=0
fi

if [ $do_prepare = 1 ]
then
  case $buildsys in 
     ubuntu|debian)
       export DEBIAN_FRONTEND=noninteractive
       export LANG=C.UTF-8
       export LC_ALL=C.UTF-8
       apt-get update -q -y
       apt-get install -y --no-install-recommends bash gcc g++ make perl libfontconfig-dev libx11-dev libxmu-dev libxaw7-dev build-essential
       apt-get install -y --no-install-recommends build-essential pkg-config libeigen3-dev libcurl4-openssl-dev libreadline-dev libboost-filesystem-dev flex libglu1-mesa-dev freeglut3-dev libosmesa6-dev libreadline6-dev zlib1g-dev bison libglm-dev libncurses-dev python3 libtirpc-dev
       ;;
     freebsd)
       env ASSUME_ALWAYS_YES=YES pkg install -y gmake gcc pkgconf libX11 libXt libXaw fontconfig perl5 eigen readline flex libGLU freeglut libosmesa zlib-ng bison glm ncurses python python3
       ;;
     *)
       echo "Unsupported build system: $buildsys" >&2
       exit 1
       ;;
  esac
fi

# special cases
export TL_MAKE=make
case "$arch" in
  *-solaris)
    export PATH=/opt/csw/bin:$PATH
    export TL_MAKE=gmake
    if [ $arch = "i386-solaris" ]
    then
      export CC="gcc -m32"
      export CXX="g++ -m32"
    else
      export CC="gcc -m64"
      export CXX="g++ -m64"
    fi
    ;;
  *-freebsd)
    export TL_MAKE=gmake
    export CC=gcc
    export CXX=g++
    export CFLAGS=-D_NETBSD_SOURCE
    export CXXFLAGS='-D_NETBSD_SOURCE -std=c++11'
    ;;
esac


find . -name \*.info -exec touch '{}' \;
touch ./utils/asymptote/camp.tab.cc
touch ./utils/asymptote/camp.tab.h
touch ./utils/asymptote/GUI/pyUIClass/*

cd utils/asymptote
./configure --prefix=/tmp/asyinst --enable-static --enable-texlive-build CXXFLAGS=-std=c++11 \
	--disable-gsl --disable-fftw --disable-lsp --disable-curl
$TL_MAKE -j2

# not necessary anymore with 2.88
## relink with static libtirpc if possible
#sed -i -e '/^LFLAGS/s/-ltirpc/-Wl,-Bstatic -ltirpc -Wl,-Bdynamic/' Makefile

# as of 2.88 necessary
# static linking of stdc++
sed -i -e '/^LFLAG/s/= /= -static-libgcc -static-libstdc++ /' Makefile
$TL_MAKE

strip asy

mv asy ../../asy-$arch

