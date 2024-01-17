#!/bin/sh -l

set -e

arch="$1"
echo "Building TL for arch = $arch"

destdir="$2"

# for CentOS we need to activate gcc-9
if [ -f /opt/rh/devtoolset-9/enable ]
then
  # we cannot call scl enable devtoolset-9 here since we need
  # the settings in the running shell
  . /opt/rh/devtoolset-9/enable
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
esac
export TL_MAKE_FLAGS

./Build -C $BUILDARGS

mv inst/bin/* $arch
tar czvf texlive-bin-$arch.tar.gz $arch
if [ -n "$destdir" ] ; then
  mv texlive-bin-$arch.tar.gz "$destdir"
  ls "$destdir"
fi

