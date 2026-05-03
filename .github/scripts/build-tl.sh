#!/bin/sh -l
# $Id$
# (-l above is to make this a login shell.)
# Public domain. Originally written by Norbert Preining.
# 
# The build script that is run by ../workflows/main.yml on github.

set -ex

if test "x$2" = "x"; then
  echo "Usage: `basename $0` arch buildsys [no-prepare]" >&2
  exit 1
fi

arch="$1"
echo "$0: Building TL for arch = $arch"
shift

buildsys=$1
shift

do_prepare=1
if test x"$1" = xno-prepare; then
  do_prepare=0
fi

echo "$0: Building on $buildsys (do_prepare=$do_prepare)"

if test $do_prepare = 1; then
  case $buildsys in 
     ubuntu|debian)
       export DEBIAN_FRONTEND=noninteractive
       export LANG=C.UTF-8
       export LC_ALL=C.UTF-8
       apt-get update -q -y
       apt-get install -y --no-install-recommends bash gcc g++ make perl libfontconfig-dev libx11-dev libxmu-dev libxaw7-dev build-essential
       ;;
     almalinux)
       yum update -y
       yum install -y gcc-toolset-11 fontconfig-devel libX11-devel libXmu-devel libXaw-devel
       . /opt/rh/gcc-toolset-11/enable
       ;;
     alpine)
       apk update
       apk add --no-progress bash gcc15-devel make perl fontconfig-dev libx11-dev libxmu-dev libxaw-dev
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
       # pkg install pkg://solaris/developer/gcc/gcc-c++-5
       /opt/csw/bin/pkgutil -U
       /opt/csw/bin/pkgutil -y -i autoconf automake gcc5core libtool
       ;;
     *)
       echo "$0: Unsupported build system: $buildsys" >&2
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
BUILDARGS=

# special cases
case "$arch" in
  armhf-linux) # debian:buster
    TL_MAKE_FLAGS="-j 1"
    export CXXFLAGS='-std=c++17'
    ;;
  aarch64-linux) # debian:buster
    BUILDARGS="--enable-arm-neon=on"
    export CXXFLAGS='-std=c++17'
    ;;
  *-solaris)
    export PATH=/opt/csw/bin:$PATH
    export TL_MAKE=gmake
    if [ $arch = "i386-solaris" ]
    then
      export CC="gcc -m32"
      export CXX="g++ -m32"
      # these commands make xdvipsk work:
      #crle -c /var/ld/ld.config -l /opt/csw/lib:/lib:/usr/lib 
      #ln -s /opt/csw/lib/i386/libstdc++.so.6 /opt/csw/lib/i386/libstdc++.so
      # 
      # But then the teckit test fails:
      # https://productionresultssa8.blob.core.windows.net/actions-results/82cefd33-f0a7-4894-865c-1bae42b5ac50/workflow-job-run-bbd5b8bc-baf2-598b-9fc7-b95d5d1633d3/logs/job/job-logs.txt?rsct=text%2Fplain&se=2026-02-11T15%3A18%3A24Z&sig=csPwLvJqdKnEHfHtFtcn6mBU5WUgaULhUDZdmlknpuM%3D&ske=2026-02-11T18%3A54%3A36Z&skoid=ca7593d4-ee42-46cd-af88-8b886a2f84eb&sks=b&skt=2026-02-11T14%3A54%3A36Z&sktid=398a6654-997b-47e9-b12b-9515b896b4de&skv=2025-11-05&sp=r&spr=https&sr=b&st=2026-02-11T15%3A08%3A19Z&sv=2025-11-05
      # 2026-02-11T15:08:00.6384675Z + ./teckit_compile ../../../libs/teckit/tex-text.map -o xtex-text.tec
      # 2026-02-11T15:08:00.6385253Z ld.so.1: teckit_compile: fatal: libstdc++.so.6: version 'GLIBCXX_3.4.29' not found (required by file teckit_compile)
      # 2026-02-11T15:08:00.6385876Z ld.so.1: teckit_compile: fatal: teckit_compile: mismatched ELF symbol versioning
      # 2026-02-11T15:08:00.6386286Z ../../../libs/teckit/teckit.test: line 7: 5876: Killed
      # 
      # So instead, let's disable xdvipsk.
      BUILDARGS=--disable-xdvipsk
    else
      export CC="gcc -m64"
      export CXX="g++ -m64"
    fi
    ;;
  *-freebsd)
    export PATH=/usr/local/bin:$PATH # for gcc15
    export TL_MAKE=gmake
    # per https://tug.org/pipermail/tlbuild/2026q2/005996.html
    # gcc14.x has only partial support for C23, despite defining
    #   options to get it, which autoconf-2.73 finds :(.
    export CC="gcc15 -Wl,-rpath,/usr/local/lib/gcc15"
    export CXX="g++15 -Wl,-rpath,/usr/local/lib/gcc15"
    export CFLAGS='-D_NETBSD_SOURCE'
    export CXXFLAGS='-D_NETBSD_SOURCE -std=c++17'
    ;;
  x86_64-linux|i386-linux|x86_64-linuxmusl)
    export CXXFLAGS='-std=c++17'
    ;;
esac
export TL_MAKE_FLAGS

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

printf "\n\f $0: build starting: `date`"
./Build -C $BUILDARGS || true # defeat sh -e
status=$?

printf "\n\f $0: build finished: `date`"
echo "$0: status = $status"
echo "$0: Here are the Work/build?*.log files:" >&2
head -n 99999 Work/build?*.log >&2

if test $status = 0; then
  echo "$0: succeeded: Build -C $BUILDARGS"
else
  echo "$0: failed: Build -C $BUILDARGS" >&2
  echo "$0: here is config.log, too:" >&2
  head -n 99999 config.log >&2
  echo "$0: aborting." >&2
  exit $status
fi

# Let's make sure that we compiled with optimization. A normal
# compilation line in the log will look like
# libtool: compile: gcc ...args... -O2 ...more args...
#
build_log=Work/build.log
if grep 'compile:.* -O' $build_log; then :; else
  echo "$0: aborting, no optimization (compile:* -O) in $build_log" >&2
  exit 1
fi

mv inst/bin/* $arch

tar czvf texlive-bin-$arch.tar.gz $arch
