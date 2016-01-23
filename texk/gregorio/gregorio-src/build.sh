#!/usr/bin/env bash
#
# Public Domain
#
# new script to build gregorio binaries (inspired from LuaTeX's one).
# ----------
# Options:
#      --mingw     : crosscompile for mingw32 from i-386linux
#      --warn      : enables all sorts of warnings
#      --host=     : target system for mingw32 cross-compilation
#      --build=    : build system for mingw32 cross-compilation
#      --arch=     : crosscompile for ARCH on OS X
#      --jobs=     : the number of jobs to run simultaneously in the make step
#      --force=    : force autoreconf or font building
#      {other)     : anything else is passed to configure verbatim
      
# try to find bash, in case the standard shell is not capable of
# handling the generated configure's += variable assignments
if which bash >/dev/null
then
 CONFIG_SHELL=`which bash`
 export CONFIG_SHELL
fi

# try to find gnu make; we may need it
MAKE=make
if make -v 2>&1| grep "GNU Make" >/dev/null
then
  echo "Your make is a GNU-make; I will use that"
elif gmake -v >/dev/null 2>&1
then
  MAKE=gmake
  export MAKE
  echo "You have a GNU-make installed as gmake; I will use that"
else
  echo "I can't find a GNU-make; I'll try to use make and hope that works." 
  echo "If it doesn't, please install GNU-make."
fi

WARNINGS=yes
MINGWCROSS=FALSE
CONFHOST=
CONFBUILD=
MACCROSS=FALSE
MAKEOPTS=
OTHERARGS=
FORCE_AUTORECONF=
FORCE_FONTS=

until [ -z "$1" ]; do
  case "$1" in
    --mingw     ) MINGWCROSS=TRUE ;;
    --host=*    ) CONFHOST="$1" ;;
    --build=*   ) CONFBUILD="$1" ;;
    --arch=*    ) MACCROSS=TRUE; ARCH=`echo $1 | sed 's/--arch=\(.*\)/\1/' ` ;;
    -j*|--jobs=*) MAKEOPTS="$MAKEOPTS $1" ;;
    --force=autoreconf) FORCE_AUTORECONF=TRUE ;;
    --force=fonts) FORCE_FONTS=TRUE ;;
    *           ) OTHERARGS="$OTHERARGS $1" ;;
  esac
  shift
done

B=build

ARCHFLAGS=

if [ "$MINGWCROSS" = "TRUE" ]
then
  MINGWBUILD=$HOSTTYPE-$OSTYPE
  MINGWSTR=mingw32
  if [ -d /usr/mingw32 ]; then
    MINGWSTR=mingw32
  else
    if [ -d /usr/i386-mingw32msvc ]; then
      MINGWSTR=i386-mingw32msvc
    else
      if [ -d /usr/i586-mingw32msvc ]; then
        MINGWSTR=i586-mingw32msvc
      fi
    fi
  fi
  OLDPATH=$PATH
  PATH=/usr/$MINGWSTR/bin:$PATH
  CFLAGS="-mtune=pentiumpro -msse2 -g -O2 $CFLAGS"
  LDFLAGS="-Wl,--large-address-aware $CFLAGS"
  ARCHFLAGS="--target=\"$MINGWSTR\" \
    --with-gnu-ld \
    --host=$MINGWSTR \
    --build=$MINGWBUILD \
    --prefix=/usr/$MINGWSTR"
else
if [ "$MACCROSS" = "TRUE" ]
then
  # make sure that architecture parameter is valid
  case $ARCH in
    i386 | x86_64 | ppc | ppc64 ) ;;
    * ) echo "ERROR: architecture $ARCH is not supported"; exit 1;;
  esac
  ARCHFLAGS="$ARCHFLAGS"
  CFLAGS="-arch $ARCH -g -O2 $CFLAGS"
  LDFLAGS="-arch $ARCH $LDFLAGS" 
fi  
fi


export CFLAGS LDFLAGS

function die {
	echo "Failed to $1."
	exit 1
}

if [ "$FORCE_AUTORECONF" = "TRUE" -o ! -e Makefile.in ]
then
  echo "Creating build files using Autotools"
  autoreconf -f -i || die "create build files"
  echo
fi

CONFIGURE_ARGS="$CONFHOST $CONFBUILD $ARCHFLAGS $OTHERARGS"
echo "Configuring build files; options: $CONFIGURE_ARGS"
./configure $CONFIGURE_ARGS || die "configure Gregorio"
echo

echo "Building Gregorio; options:$MAKEOPTS"
${MAKE} ${MAKEOPTS} || die "build Gregorio"
echo

if [ "$FORCE_FONTS" = "TRUE" -o ! -e fonts/greciliae.ttf ]
then
  echo "Building fonts; options:$MAKEOPTS"
  cd fonts
  ${MAKE} ${MAKEOPTS} fonts || die "build fonts"
  cd ..
  echo
fi

if [ "$MINGWCROSS" = "TRUE" ]
then
  PATH=$OLDPATH
fi

echo "Build complete.  Next, you may want to run ./install.sh to install."
echo
echo "Depending on installation directory, you probably need to run"
echo "./install.sh using sudo or as root."

exit 0
