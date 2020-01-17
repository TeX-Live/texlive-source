#!/bin/sh
# Public domain. Originally written by Norbert Preining and Karl Berry, 2018.
# Note from Paulo: this script provides better Cygwin support than our original
# approach, so the team decided to use it as a proper wrapper for texplate as well.

scriptname=`basename "$0"`
jar="$scriptname.jar"
jarpath=`kpsewhich --progname="$scriptname" --format=texmfscripts "$jar"`

kernel=`uname -s 2>/dev/null`
if echo "$kernel" | grep CYGWIN >/dev/null; then
  CYGWIN_ROOT=`cygpath -w /`
  export CYGWIN_ROOT
  jarpath=`cygpath -w "$jarpath"`
fi

exec java -jar "$jarpath" "$@"