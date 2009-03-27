#!/bin/sh
srcdir=$1
if [ -e "`which svnversion`" ] && [ -e "`which sed`" ]; then
    REV=`svnversion $srcdir | sed 's/^\([0-9]\+\).*/\1/'`
    case $REV in *[^0-9]*)
        echo unknown
        exit 0
        ;;
    esac
    echo $REV
else
    echo unknown
fi