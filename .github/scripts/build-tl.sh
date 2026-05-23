#!/bin/sh -l
# $Id$
# (-l above is to make this a login shell.)
# Public domain. Originally written by Norbert Preining.
# 
# The build script that is run by ../workflows/main.yml on github.

set -ex

if test "x$2" = "x"; then
  echo "Usage: `basename $0` TLARCH BUILDSYS [no-prepare]" >&2
  echo "TLARCH = TeX Live platform name"
  echo "BUILDSYS = general distro name we will build on"
  echo "no-prepare = omit BUILDSYS checks, to run locally on another system"
  exit 1
fi

arch=$1; shift
echo "$0: Building TL for arch $arch"

buildsys=$1; shift

do_prepare=1
if test x"$1" = xno-prepare; then
  do_prepare=0
fi

# emacs-page
echo "$0: Building on $buildsys (do_prepare=$do_prepare)"

if test $do_prepare = 1; then
  case $buildsys in 
     ubuntu|debian)
       export DEBIAN_FRONTEND=noninteractive
       export LANG=C.UTF-8
       export LC_ALL=C.UTF-8
       apt-get update -q -y
       apt-get install -y --no-install-recommends bash gcc g++ make perl \
                            libfontconfig-dev libx11-dev libxmu-dev \
                            libxaw7-dev build-essential
       ;;
     almalinux)
       yum update -y
       yum install -y gcc-toolset-15 fontconfig-devel perl-interpreter \
                      libX11-devel libXmu-devel libXaw-devel
       . /opt/rh/gcc-toolset-15/enable
       ;;
     alpine) # aka musl
       apk update
       apk add --no-progress bash gcc g++ make musl-dev perl fontconfig-dev libx11-dev libxmu-dev libxaw-dev
       ;;
     freebsd)
       env ASSUME_ALWAYS_YES=YES pkg install -y gmake gcc15-devel pkgconf libX11 libXt libXaw fontconfig perl5
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

# emacs-page
# default settings
TL_MAKE_FLAGS="-j 2"
BUILDARGS=

# special cases
case "$arch" in
  armhf-linux)
    TL_MAKE_FLAGS="-j 1"
    ;;
  aarch64-linux)
    BUILDARGS="--enable-arm-neon=on"
    ;;
  x86_64-linuxmusl)
    # alpine3.22 provides only gcc14, and gcc14 is problematic,
    # as explained under freebsd below. Force use of older C standard.
    export CFLAGS="-std=gnu11"
    ;;
  *-solaris)
    export PATH=/opt/csw/bin:$PATH
    export TL_MAKE=gmake
    if test $arch = "i386-solaris"; then
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
      # So instead, let's disable xdvipsk on i386-solaris.
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
    #   So we need gcc15. See more comments in main.yml.
    export CC="gcc15 -Wl,-rpath,/usr/local/lib/gcc15"
    export CXX="g++15 -Wl,-rpath,/usr/local/lib/gcc15"
    export CFLAGS='-D_NETBSD_SOURCE'
    export CXXFLAGS='-D_NETBSD_SOURCE'
    ;;
esac
export TL_MAKE_FLAGS

# ICU requires C++17, so we always need it.
export CXXFLAGS="$CXXFLAGS -std=c++17"

# Report the compiler version.
echo "$0: checking \$CC --version:"
${CC-gcc} --version || true # defeat -e, configure will fail anyway

showfile() {
  for f in "$@"; do
    echo "$0: ==> $f"
    cat $f
    echo "$0: end $f <=="
  done
}

# Make binaries harder to exploit with -fhardened. This option is only
# supported on GNU/Linux, and only as of GCC 15. Older versions have
# some support, but we'd have to specify a bunch of explicit options,
# which seems a recipe for unnecessary maintenance pain.
# 
# Instead of hardwiring version numbers and platforms, try a test
# compilation.
# 
touch empty.c
# Let's check -fsanitize=undefined again after we update the
# x86_64-linux build system. On Alma, it seems the necessary libubsan.a
# is not installed as part of gcc-toolset-15. Also, we'll need to use
# a hello,world test program instead of an empty file.
for option in -fhardened; do
  echo "$0: checking whether we can enable $option"
  # Optimization is required for these; e.g., warnings about _FORTIFY_SOURCE.
  if ${CC-gcc} $CFLAGS -O2 $option -c empty.c >empty.out 2>&1; then
    # Although these are only warnings if not supported, e.g., on freebsd,
    # it's too annoying to see the warning on every compile line.
    if test -s empty.out; then
      echo "$0: empty.c $option compilation got diagnostics, not setting."
      showfile empty.out
    else
      echo "$0: empty.c $option compilation successful, setting."
      export CFLAGS="$CFLAGS $option"
    fi
  else
    echo "$0: empty.c $option compilation failed, not setting."
    showfile empty.out
  fi
done

# It's up to us to enable optimization if we are overriding the
# compilation flags.
test -z "$CFLAGS" || CFLAGS="$CFLAGS -O2"
test -z "$CXXFLAGS" || CXXFLAGS="$CXXFLAGS -O2"

echo "$0: variables set:"
echo "  BUILDARGS=$BUILDARGS"
echo "  CC=$CC"
echo "  CXX=$CXX"
echo "  CFLAGS=$CFLAGS"
echo "  CXXFLAGS=$CXXFLAGS"
echo "  TL_MAKE=$TL_MAKE"
echo "  TL_MAKE_FLAGS=$TL_MAKE_FLAGS"
echo "$0: (end variables)."

# emacs-page
printf "\n\f $0: build starting: `date`"
if ./Build -C $BUILDARGS; then # thanks to -e; keep exit status without exiting
  status=$?
else
  status=$?
fi
printf "\n\f $0: build finished: `date`"
echo "$0: status = $status"
echo "$0: Here are the Work/build?*.log files:" >&2
showfile Work/build?*.log >&2

if test $status = 0; then
  echo "$0: succeeded: Build -C $BUILDARGS"
  # continue below.
else
  echo "$0: failed: Build -C $BUILDARGS" >&2
  showfile Work/config.log >&2
  echo "$0: aborting with status $status." >&2
  exit $status
fi

# Let's make sure that we compiled with optimization. A normal
# compilation line in the log will look like
# libtool: compile: gcc ...args... -O2 ...more args...
#
build_log=Work/build.log
if grep 'compile:.* -O' $build_log; then :; else
  echo "$0: aborting, no optimization /compile:.* -O/ in $build_log" >&2
  exit 1
fi

mv inst/bin/* $arch

tar czvf texlive-bin-$arch.tar.gz $arch
