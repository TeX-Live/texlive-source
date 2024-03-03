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
       apt-get install -y --no-install-recommends build-essential pkg-config libeigen3-dev libcurl4-openssl-dev libreadline-dev libboost-filesystem-dev flex libglu1-mesa-dev freeglut3-dev libosmesa6-dev libreadline6-dev zlib1g-dev bison libglm-dev libncurses-dev python3
       ;;
     *)
       echo "Unsupported build system: $buildsys" >&2
       exit 1
       ;;
  esac
fi

find . -name \*.info -exec touch '{}' \;
touch ./utils/asymptote/camp.tab.cc
touch ./utils/asymptote/camp.tab.h
touch ./utils/asymptote/GUI/pyUIClass/*

cd utils/asymptote
./configure --prefix=/tmp/asyinst --enable-static --enable-texlive-build CXXFLAGS=-std=c++11 \
	--disable-gsl --disable-fftw --disable-lsp --disable-curl
make -j2
strip asy

mv asy ../../asy-$arch

