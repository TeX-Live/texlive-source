#!/bin/sh -l
# $Id$
# (-l above is to make this a login shell.)
# Build script for asymptote on github. Norbert Preining. Public domain.

set -ex

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
       # asymptote uses lots of packages.
       packages="bash bison build-essential cmake flex freeglut3-dev
                 g++ gcc libboost-filesystem-dev libeigen3-dev libfftw3-dev
                 libfontconfig-dev libglm-dev libglu1-mesa-dev
                 libncurses-dev libosmesa6-dev
                 libreadline-dev libreadline6-dev libtirpc-dev
                 libtool-bin libx11-dev libxaw7-dev libxmu-dev make perl
                 pkg-config python3 zlib1g-dev"
       apt-get install -y --no-install-recommends $packages
       ;;
     freebsd)
       packages="bison cmake eigen fftw flex fontconfig freeglut gcc
                 glm gmake libGLU libX11 libXaw libXt libosmesa libtool
                 ncurses perl5 pkgconf python python3 readline zlib-ng"
       env ASSUME_ALWAYS_YES=YES pkg install -y $packages
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
    export CXXFLAGS=-D_NETBSD_SOURCE
    ;;
esac

# If we explicitly set CFLAGS or CXXFLAGS above, it's up to us to enable
# optimization, since we are overriding what Autoconf does.
test -n "$CFLAGS" && CFLAGS="$CFLAGS -O2"
test -n "$CXXFLAGS" && CXXFLAGS="$CXXFLAGS -O2"

echo "$0: variables set:"
echo "  BUILDARGS=$BUILDARGS"
echo "  CC=$CC"
echo "  CXX=$CXX"
echo "  CFLAGS=$CFLAGS"
echo "  CXXFLAGS=$CXXFLAGS"
echo "  TL_MAKE=$TL_MAKE"
echo "  TL_MAKE_FLAGS=$TL_MAKE_FLAGS"
echo "$0: (end variables)."

find . -name \*.info -exec touch '{}' \;
touch ./utils/asymptote/camp.tab.cc
touch ./utils/asymptote/camp.tab.h
touch ./configure ./Makefile.in

# About the disabled features:
# libcurl -> libpsl -> gss and more, too much.
# libgsl -> undefined reference to `_dl_x86_cpu_features', apparently
#   can resolve only by statically linking glibc, which is worse.
# 
cd utils/asymptote
sh -vx ./configure \
  --prefix=/tmp/asyinst --enable-static --enable-texlive-build \
  --disable-gsl --disable-curl \
  LDFLAGS="-static-libgcc -static-libstdc++"
$TL_MAKE SILENT_MAKE= -j2

strip asy
mv asy ../../asy-$arch
