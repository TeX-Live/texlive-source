#!/bin/sh -l

set -e

if [ "x$2" = "x" ]
then
  echo "Usage: `basename $0` arch buildsys [steps]" >&2
  exit 1
fi

arch="$1"
echo "Building TL for arch = $arch"
shift

buildsys=$1
echo "Building on $buildsys"
shift

STEPS=",$1,"
shift
if [ "x$STEPS" = "x" ]
  STEPS=",prepare,build,"
fi

destdir="$1"
shift

if [[ "$STEPS" == *,prepare,* ]]
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
     freebsd|netbsd)
       pkg install gmake gcc pkgconf libX11 libXt libXaw fontconfig
       ;;
     solaris)
       echo "Solaris support is WIP, please help!" >&2
       exit 1
     *)
       echo "Unsupported build system: $buildsys" >&2
       exit 1
  esac
fi

if [[ "$STEPS" == *,build,* ]]
then
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
      export CC="/path/to/gcc-5.5 -m64"
      export CXX="/path/to/g++-5.5 -m64"
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
  tar czvf texlive-bin-$arch.tar.gz $arch
fi
