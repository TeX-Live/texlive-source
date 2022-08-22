#! /bin/sh

# Copyright (C) 2001-2019 Peter Selinger.
# This file is part of Potrace. It is free software and it is covered
# by the GNU General Public License. See the file COPYING for details.

echo "Checking backends..." >& 2

if test "$#" -gt 0 -a "$1" = "-t"; then
    testflag=yes
    shift
fi

# we cannot check the output exactly because of floating point
# differences on different architectures. So we content ourselves with
# checking the file type and the approximate file size. Note that the
# file size varies considerably under Windows due to the newline
# coding. The main point of this test is to exercise as many parts of
# the backend code as possible.

if test -z "$srcdir"; then
    srcdir=.
fi

. "$srcdir/missing.sh"

NAME=`basename "$0"`

POTRACE="${CHECK_POTRACE:-../src/potrace$EXEEXT --progress}"
DATADIR="$srcdir/data"
TMPDIR="${TEMPDIR:-/tmp}"
TMP1=`mktemp "$TMPDIR/$NAME-1.XXXXXX"`
DATA="$DATADIR/data1.pbm"
DATASIZE=5956

# run the command, expecting return value 0
action () {
    "$@"
    if test "$?" -ne 0; then
	echo "$NAME: test failed" >& 2
	echo "Failed command: $LINE: $@" >& 2
	exit 1
    fi
}

# keep track of line numbers
alias action="LINE=\$LINENO; action"

# we need a portable, fast way of determining the size of a file
getsize () {
    F="$1"
    set dummy `ls -l "$DATA"`
    if test "$6" = "$DATASIZE"; then
        set dummy `ls -l "$F"`
	echo "$6"
    elif test "$5" = "$DATASIZE"; then
	set dummy `ls -l "$F"`
	echo "$5"
    else
	set dummy `wc "$F"`
	echo "$4"
    fi
}

# check the first line of the file against template
filetest () {
    F="$1"
    T="$2"
    head -n 1 "$F" | grep "$T"
}

# check that the file size is within given bounds
sizetest () {
    F="$1"
    L="$2"
    U="$3"
    S=`getsize "$F"`
    if test "$testflag" = "yes"; then
	echo "Size: $S, Bounds: $L $U" >& 2
	return 0;
    fi
    if test "$S" -lt "$L" -o "$S" -gt "$U"; then
	echo "File size: $S" >& 2
	return 1;
    fi
    return 0;
}


# test eps
action $POTRACE -o "$TMP1" "$DATA"
action filetest "$TMP1" '^%!PS-Adobe-3.0 EPSF-3.0$' > /dev/null
# omit size test as it depends on compile-time defaults

# test eps with cleartext
action $POTRACE -c -o "$TMP1" "$DATA"
action filetest "$TMP1" '^%!PS-Adobe-3.0 EPSF-3.0$' > /dev/null
action sizetest "$TMP1" 9000 9800

# test eps with level 2 compression
action $POTRACE -2 -o "$TMP1" "$DATA"
action filetest "$TMP1" '^%!PS-Adobe-3.0 EPSF-3.0$' > /dev/null
action sizetest "$TMP1" 5500 5900

# test eps with level 3 compression
R=`action $POTRACE -3 -o "$TMP1" "$DATA" 2>&1`
if test "$?" -ne 0; then
    echo "$R" >& 2
    exit 1
fi
if echo "$R" | grep 'option -3 not supported' > /dev/null ; then
  true # do nothing
else
  action filetest "$TMP1" '^%!PS-Adobe-3.0 EPSF-3.0$' > /dev/null
  action sizetest "$TMP1" 5200 5550
fi

# test debugging backends
action $POTRACE -c -d1 -o "$TMP1" "$DATA"
action filetest "$TMP1" '^%!PS-Adobe-3.0 EPSF-3.0$' > /dev/null
action sizetest "$TMP1" 400300 433000

action $POTRACE -c -d2 -o "$TMP1" "$DATA"
action filetest "$TMP1" '^%!PS-Adobe-3.0 EPSF-3.0$' > /dev/null
action sizetest "$TMP1" 150300 161700

action $POTRACE -c -d3 -o "$TMP1" "$DATA"
action filetest "$TMP1" '^%!PS-Adobe-3.0 EPSF-3.0$' > /dev/null
action sizetest "$TMP1" 169600 182300

action $POTRACE -c -d4 -o "$TMP1" "$DATA"
action filetest "$TMP1" '^%!PS-Adobe-3.0 EPSF-3.0$' > /dev/null
action sizetest "$TMP1" 227400 243900

# test ps
action $POTRACE -b ps -c -o "$TMP1" "$DATA"
action filetest "$TMP1" '^%!PS-Adobe-3.0$' > /dev/null
action sizetest "$TMP1" 9000 9850

# test pdf
action $POTRACE -b pdf -c -o "$TMP1" "$DATA"
action filetest "$TMP1" '^%PDF-1.3$' > /dev/null
action sizetest "$TMP1" 16800 18000

# test svg
action $POTRACE -b svg -o "$TMP1" "$DATA"
action filetest "$TMP1" '^<?xml version="1.0" standalone="no"?>$' > /dev/null
action sizetest "$TMP1" 12000 13850

# test svg debugging backend
action $POTRACE -b svg -d1 -o "$TMP1" "$DATA"
action filetest "$TMP1" '^<?xml version="1.0" standalone="no"?>$' > /dev/null
action sizetest "$TMP1" 21050 21500

# test pgm
action $POTRACE -b pgm -o "$TMP1" "$DATA"
action filetest "$TMP1" 'P5' > /dev/null
action sizetest "$TMP1" 45300 45400

# test gimppath
action $POTRACE -b gimp -o "$TMP1" "$DATA"
action filetest "$TMP1" '^<?xml version="1.0" standalone="no"?>$' > /dev/null
action sizetest "$TMP1" 11000 12500

# test xfig
action $POTRACE -b xfig -o "$TMP1" "$DATA"
action filetest "$TMP1" '#FIG 3.2' > /dev/null
action sizetest "$TMP1" 20000 23150

# test dxf
action $POTRACE -b dxf -o "$TMP1" "$DATA"
action filetest "$TMP1" '^ *999$' > /dev/null
action sizetest "$TMP1" 120000 160000

# test geojson
action $POTRACE -b geojson -o "$TMP1" "$DATA"
action filetest "$TMP1" '^{$' > /dev/null
action sizetest "$TMP1" 68000 69000

action rm -f "$TMP1"

echo "$NAME: test succeeded" >& 2
exit 0
