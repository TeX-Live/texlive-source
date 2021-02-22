#!/bin/bash
# Public domain. Originally written by Norbert Preining and Karl Berry, 2018-2020.

scriptname=`basename "$0"`
javaVersion=$(java -version 2>&1 | head -1)
case "$javaVersion" in
  *"1.8"*) jar="$scriptname-jdk8".jar ;;
  *) jar="$scriptname.jar" ;;
esac
jarpath=`kpsewhich --progname="$scriptname" --format=texmfscripts "$jar"`

kernel=`uname -s 2>/dev/null`
if echo "$kernel" | grep CYGWIN >/dev/null; then
  CYGWIN_ROOT=`cygpath -w /`
  export CYGWIN_ROOT
  jarpath=`cygpath -w "$jarpath"`
fi

exec java -jar "$jarpath" "$@"

