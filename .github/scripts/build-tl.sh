#!/bin/sh -l

set -e

if [ "x$2" = "x" ]
then
  echo "Usage: `basename $0` arch buildsys [no-prepare]" >&2
  exit 1
fi

arch="$1"
echo "Building TL for arch = $arch"
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
       ;;
     centos)
       yum update -y
       yum install -y centos-release-scl
       yum install -y devtoolset-9 fontconfig-devel libX11-devel libXmu-devel libXaw-devel
       . /opt/rh/devtoolset-9/enable
       ;;
     alpine)
       apk update
       apk add --no-progress bash gcc g++ make perl fontconfig-dev libx11-dev libxmu-dev libxaw-dev
       ;;
     freebsd)
       env ASSUME_ALWAYS_YES=YES pkg install -y gmake gcc pkgconf libX11 libXt libXaw fontconfig perl5
       ;;
     netbsd)
       pkg_add gmake gcc pkgconf libX11 libXt libXaw fontconfig perl5
       ;;
     solaris)
       # pkg install pkg://solaris/developer/gcc-5
       # maybe only the following is enough, and fortran and gobjc needs not be installed?
       pkg install pkg://solaris/developer/gcc/gcc-c++-5
       ;;
     *)
       echo "Unsupported build system: $buildsys" >&2
       exit 1
       ;;
  esac
fi

find . -name \*.info -exec touch '{}' \;
touch ./texk/detex/detex-src/detex.c
touch ./texk/detex/detex-src/detex.h
touch ./texk/gregorio/gregorio-src/src/gabc/gabc-score-determination.c
touch ./texk/gregorio/gregorio-src/src/gabc/gabc-score-determination.h
touch ./texk/gregorio/gregorio-src/src/vowel/vowel-rules.h
touch ./texk/web2c/omegafonts/pl-lexer.c
touch ./texk/web2c/omegafonts/pl-parser.c
touch ./texk/web2c/omegafonts/pl-parser.h
touch ./texk/web2c/otps/otp-lexer.c
touch ./texk/web2c/otps/otp-parser.c
touch ./texk/web2c/otps/otp-parser.h
touch ./texk/web2c/web2c/web2c-lexer.c
touch ./texk/web2c/web2c/web2c-parser.c
touch ./texk/web2c/web2c/web2c-parser.h
touch ./utils/asymptote/camp.tab.cc
touch ./utils/asymptote/camp.tab.h
touch ./utils/lacheck/lacheck.c
touch ./utils/xindy/xindy-src/tex2xindy/tex2xindy.c
# sometimes dvipng.1 seems to be outdated
touch ./texk/dvipng/doc/dvipng.1
touch ./texk/dvipng/dvipng-src/dvipng.1

# default settings
TL_MAKE_FLAGS="-j 2"
BUILDARGS=""

# special cases
case "$arch" in
  armhf-linux)
    TL_MAKE_FLAGS="-j 1"
    ;;
  aarch64-linux)
    BUILDARGS="--enable-arm-neon=on"
    ;;
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
export TL_MAKE_FLAGS

./Build -C $BUILDARGS

mv inst/bin/* $arch

#
# Build asy if possible
#
case $buildsys in 
   ubuntu|debian)
     export DEBIAN_FRONTEND=noninteractive
     apt-get install -y --no-install-recommends build-essential pkg-config libeigen3-dev libcurl4-openssl-dev libreadline-dev libboost-filesystem-dev libtirpc-dev flex libglu1-mesa-dev freeglut3-dev libosmesa6-dev libreadline6-dev zlib1g-dev bison libglm-dev libncurses-dev
     cd utils/asymptote
     ./configure --prefix=/tmp/asyinst --enable-static --enable-texlive-build CXXFLAGS=-std=c++11 --disable-gsl --disable-fftw --disable-lsp
     sed -i -e 's/^LIBS = /LIBS = -static-libgcc -static-libstdc++ /' Makefile
     make -j2
     strip asy
     cp asy ../../$arch.
     ;;
esac

    



tar czvf texlive-bin-$arch.tar.gz $arch
