#!/bin/sh
# Adapted from tlcockpit.sh to ensure the script works with cygwin

scriptname=`basename "$0" .sh`
jar="$scriptname.jar"
jarpath=`kpsewhich --progname="$scriptname" --format=texmfscripts "$jar"`

kernel=`uname -s 2>/dev/null`
if echo "$kernel" | grep CYGWIN >/dev/null; then
  CYGWIN_ROOT=`cygpath -w /`
  export CYGWIN_ROOT
  jarpath=`cygpath -w "$jarpath"`
fi

# User may have globally set their locale provider preference in
# $JAVA_TOOL_OPTIONS so don't override it.

if [ -z "$JAVA_TOOL_OPTIONS" ]; then
  exec java -Djava.locale.providers=CLDR,JRE,SPI -jar "$jarpath" "$@"
else
  exec java -jar "$jarpath" "$@"
fi
