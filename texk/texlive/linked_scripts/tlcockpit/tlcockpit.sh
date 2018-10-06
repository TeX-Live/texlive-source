#!/bin/sh
# Public domain. Originally written by Norbert Preining and Karl Berry, 2018.

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

